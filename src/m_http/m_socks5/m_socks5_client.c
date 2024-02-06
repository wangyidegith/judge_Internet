#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "../m_net/m_net.h"

#include "m_socks5.h"


typedef unsigned char byte;
int socks5_client(const char* PROXY_IP, int PROXY_PORT, const char* USERNAME, const char* PASSWORD, const char* TARGET_ADDR, const int TARGET_PORT) {
	// First
	int conn_proxy__fd = createConnectSocket(PROXY_IP, PROXY_PORT);
	if (conn_proxy__fd == -1) {
		return -1;
	}
	// Second
	// 1
	// send the first bao
	//     VERSION(1) NMETHOD(1) METHODS(NMETHOD)
	byte request1[] = {SOCKS5_VERSION, 0x01, SOCKS5_AUTH_METHOD_USERNAME_PASSWORD};
	/*
	   for (int i = 0; i < sizeof(request1); i++) {
	   printf("req1[%d] is %u\n", i, request1[i]);
	   }
	   */
	if (writen(conn_proxy__fd, (char*)request1, sizeof(request1)) == -1) {
		perror("write(conn_proxy__fd, request1)");
		return -1;
	}
	// recv the first bao
	//     VERSION(1) METHOD(1)
	byte response1[2] = {"\0"};
	int r_ret = readn(conn_proxy__fd, (char*)response1, sizeof(response1));
	if (r_ret < 0) {
		perror("read(conn_proxy__fd, response1)");
		return -1;
	} else if (r_ret == 0) {
		fprintf(stderr, "proxy %s:%d closed.\n", PROXY_IP, PROXY_PORT);
		return -1;
	}
	/*
	   printf("recv the first response:\n");
	   for (int i = 0; i < sizeof(response1); i++) {
	   printf("recv1[%d] is %u\n", i, response1[i]);
	   }
	   ptintf("-------\n");
	   */
	// check recv
	if (response1[0] != SOCKS5_VERSION || response1[1] != SOCKS5_AUTH_METHOD_USERNAME_PASSWORD) {
		fprintf(stderr, "The first authentication failed.\n");
		return -1;
	}
	// 2
	// send username and passwd
	//     VERSION(1) ULEN(1) USERNAME(ULEN) PLEN(1) PASSWORD(PLEN)
	byte username_len = (byte)strlen(USERNAME);
	byte password_len = (byte)strlen(PASSWORD);
	byte auth_credentials[3 + username_len + password_len];
	auth_credentials[0] = 0x01;
	auth_credentials[1] = username_len;
	memcpy((void*)(auth_credentials + 2), (void*)USERNAME, username_len);
	auth_credentials[2 + username_len] = password_len;
	memcpy((void*)(auth_credentials + 2 + username_len + 1), (void*)PASSWORD, password_len);
	/*
	   for (int i = 0; i < sizeof(auth_credentials); i++) {
	   printf("req2[%d] is %u\n", i, auth_credentials[i]);
	   }
	   */
	if (writen(conn_proxy__fd, (char*)auth_credentials, sizeof(auth_credentials)) == -1) {
		perror("write(conn_proxy__fd, auth_credentials)");
		return -1;
	}
	// recv
	//     VERSION(1) STATUS(1)
	//         STATUS == 0 is success
	byte auth_result[2];
	if (readn(conn_proxy__fd, (char*)auth_result, sizeof(auth_result)) == -1) {
		perror("read(conn_proxy__fd, auth_result)");
		return -1;
	}
	/*
	   printf("recv the second response:\n");
	   for (int i = 0; i < sizeof(auth_result); i++) {
	   printf("recv2[%d] is %u\n", i, auth_result[i]);
	   }
	   printf("-------\n");
	   */
	// check recv
	if (auth_result[0] != 0x01 || auth_result[1] != 0x00) {
		fprintf(stderr, "Username/password authentication failed.\n");
		return -1;
	}
	// -------3
	// send target info
	//     VERSION(1)
	//     CMD(1)
	//         0x01 (CONNECT), 0x02 (BIND, reverse proxy), 0x03 (UDP ASSOCIATE)
	//     RSV(1)
	//         reserve, default value is 0x00
	//     ATYP(1)
	//         0x01 (IPv4), 0x03 (domain), 0x04 (IPv6)
	//     DST.ADDR(at least 1)
	//         IPv4(4)
	//         domain(the first byte value)
	//         IPv6(16)
	//     DST.PORT(2)
	byte* target_info_pkt = (byte*)malloc(6);
	byte low_byte = TARGET_PORT & 0xFF;
	byte high_byte = (TARGET_PORT >> 8) & 0xFF;
	size_t target_info_pkt_len = 0;
	if (inet_pton(AF_INET, TARGET_ADDR, &(target_info_pkt[4])) > 0) {
		// IPv4 4
		target_info_pkt[3] = SOCKS5_ADDR_TYPE_IPV4;
		target_info_pkt = (byte*)realloc((void*)target_info_pkt, 6 + 4);
		target_info_pkt[4 + 4] = low_byte;
		target_info_pkt[4 + 4 + 1] = high_byte;
		// target_info_pkt_len = 10;
	} else if (inet_pton(AF_INET6, TARGET_ADDR, &(target_info_pkt[4])) > 0) {
		// IPv6 16
		target_info_pkt[3] = SOCKS5_ADDR_TYPE_IPV6;
		target_info_pkt = (byte*)realloc((void*)target_info_pkt, 6 + 16);
		target_info_pkt[4 + 16] = low_byte;
		target_info_pkt[4 + 16 + 1] = high_byte;
		target_info_pkt_len = 22;
	} else {
		// domain strlen(TRAGET_IP)
		target_info_pkt[3] = SOCKS5_ADDR_TYPE_DOMAIN;
		byte domain_len = (byte)strlen(TARGET_ADDR);
		target_info_pkt = (byte*)realloc((void*)target_info_pkt, 6 + 1 + domain_len);
		target_info_pkt[4] = domain_len;
		memcpy((void*)target_info_pkt + 4 + 1, (void*)TARGET_ADDR, domain_len);
		target_info_pkt[4 + 1 + domain_len] = high_byte;
		target_info_pkt[4 + 1 + domain_len + 1] = low_byte;
		target_info_pkt_len = 7 + domain_len;
	}
	target_info_pkt[0] = SOCKS5_VERSION;
	target_info_pkt[1] = SOCKS5_CMD_TYPE_CONNECT;
	target_info_pkt[2] = 0x00;
	/*
	   for (int i = 0; i < target_info_pkt_len; i++) {
	   printf("req3[%d] is %u\n", i, target_info_pkt[i]);
	   }
	   */
	if (writen(conn_proxy__fd, (char*)target_info_pkt, target_info_pkt_len) == -1) {
		perror("write(conn_proxy__fd, target_info_pkt_len)");
		return -1;
	}
	// recv
	byte response2[22] = {"\0"};
	r_ret = read(conn_proxy__fd, (void*)response2, sizeof(response2));
	if (r_ret < 0) {
		perror("read(conn_proxy__fd, response1)");
		return -1;
	} else if (r_ret == 0) {
		fprintf(stderr, "proxy %s:%d closed.\n", PROXY_IP, PROXY_PORT);
		return -1;
	}
	// check recv
	if (response2[1] != 0x00) {
		fprintf(stderr, "Connect details authentiction failed.\n");
		return -1;
	}
	return conn_proxy__fd;
}
