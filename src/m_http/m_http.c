#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "m_net/m_net.h"
#include "m_url_process/m_url_process.h"
#include "m_socks5/m_socks5.h"

#include "m_http.h"


int httpGet(const char* target_url, char* response) {
	char res_path[MAX_BUF_SIZE] = "\0";
	if (getResPathFromUrl(target_url, res_path, sizeof(res_path))) {
		return -1;
	}
	char hostname[MAX_BUF_SIZE] = "\0";
	if (getHostnameFromUrl(target_url, hostname, sizeof(hostname))) {
		return -1;
	}
	char server_ip[INET6_ADDRSTRLEN] = "\0";
	if (getIPFromUrl(target_url, server_ip, sizeof(server_ip))) {
		return -1;
	}
	int server_port = 80;
	int conn_fd = createConnectSocket(server_ip, server_port);
	if (conn_fd == -1) {
		return -1;
	}
	char request[HTTP_SEND_BUF_SIZE] = "\0";
	snprintf(request, HTTP_SEND_BUF_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", res_path, hostname);
	if (writen(conn_fd, request, strlen(request)) < 0) {
		perror("writen(request)");
		return -1;
	}
	int bytes_received;
	bytes_received = read(conn_fd, response, HTTP_RECV_BUF_SIZE - 1);
	if (bytes_received < 0) {
		perror("read(response)");
		return -1;
	} else if (bytes_received == 0) {
		printf("peer closed.\n");
		return -1;
	}
	close(conn_fd);
	return 0;
}

int httpGetOverProxy(const char* target_url, char* response, const char* proxy_url) {
	// 1 resource prepare
	char PROXY_IP[MAX_BUF_SIZE] = "\0";
	if (getIPFromUrl(proxy_url, PROXY_IP, sizeof(PROXY_IP))) {
		return -1;
	}
	int PROXY_PORT;
	if (getPortFromUrl(proxy_url, &PROXY_PORT)) {
		return -1;
	}
	char USERNAME[MAX_BUF_SIZE] = "\0";
	if (getUsernameFromUrl(proxy_url, USERNAME, sizeof(USERNAME))) {
		return -1;
	}
	char PASSWORD[MAX_BUF_SIZE] = "\0";
	if (getPasswdFromUrl(proxy_url, PASSWORD, sizeof(PASSWORD))) {
		return -1;
	}
	char TARGET_ADDR[MAX_BUF_SIZE] = "\0";
	if (getHostnameFromUrl(target_url, TARGET_ADDR, sizeof(TARGET_ADDR))) {
		return -1;
	}
	// 2 network connect
	int conn_proxy__fd = socks5_client(PROXY_IP, PROXY_PORT, USERNAME, PASSWORD, TARGET_ADDR, 80);
	if (conn_proxy__fd < 0) {
		return -1;
	}
	// 3 success, you can start send data
	char RES_PATH[MAX_BUF_SIZE] = "\0";
	if (getResPathFromUrl(target_url, RES_PATH, sizeof(RES_PATH))) {
		return -1;
	}
	char request[HTTP_SEND_BUF_SIZE] = "\0";
	snprintf(request, HTTP_SEND_BUF_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RES_PATH, TARGET_ADDR);
	if (writen(conn_proxy__fd, request, strlen(request)) < 0) {
		perror("writen(conn_proxy__fd, request)");
		return -1;
	}
	int bytes_received;
	bytes_received = read(conn_proxy__fd, response, HTTP_RECV_BUF_SIZE - 1);
	if (bytes_received < 0) {
		perror("read(conn_proxy__fd, response)");
		return -1;
	} else if (bytes_received == 0) {
		printf("peer closed.\n");
		return -1;
	}
	close(conn_proxy__fd);
	return 0;
}

void getContentLengthFromHTTPResponse(const char* response, int* content_length) {
	const char* start = strstr(response, "Content-Length: ");
	start += strlen("Content-Length: ");
	sscanf(start, "%d", content_length);
}

void getMessageFromHTTPResponse(const char* response, char* message) {
	const char* start = strstr(response, "\r\n\r\n");
	start += strlen("\r\n\r\n");
	int content_length;
	getContentLengthFromHTTPResponse(response, &content_length);
	strncpy(message, start, content_length);
}
