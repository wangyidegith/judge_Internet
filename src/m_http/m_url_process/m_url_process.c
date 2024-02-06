#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "m_url_process.h"

int getProtoFromUrl(const char* target_url, char* proto, int max_len) {
	int actual_len;
	const char* prefix_flag = "://";
	char* cur_target_url = NULL;
	cur_target_url = strstr(target_url, prefix_flag);
	if (!cur_target_url) {
		return -1;
	}
	actual_len = cur_target_url - target_url;
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)proto, (void*)target_url, actual_len);
	proto[max_len - 1] = '\0';
	return 0;
}

int getBodyFromUrl(const char* target_url, char* body, int max_len) {
	int actual_len;
	const char* prefix_flag = "://";
	char* cur_target_url = NULL;
	cur_target_url = strstr(target_url, prefix_flag);
	if (!cur_target_url) {
		return -1;
	}
	cur_target_url += 3;
	actual_len = strlen(cur_target_url);
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)body, (void*)cur_target_url, actual_len);
	body[max_len - 1] = '\0';
	return 0;
}

int getHostnameStartFromUrl(const char* target_url, char* hostname_start, int max_len) {
	int actual_len;
	char body[MAX_BUF_SIZE];
	memset((void*)body, 0x00, sizeof(body));
	if (getBodyFromUrl(target_url, body, sizeof(body))) {
		return -1;
	}
	char* cur_body_url = NULL;
	cur_body_url = strchr(body, '@');
	if (!cur_body_url) {
		cur_body_url = body;
	} else {
		cur_body_url += 1;
	}
	actual_len = strlen(cur_body_url);
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)hostname_start, (void*)cur_body_url, actual_len);
	hostname_start[max_len - 1] = '\0';
	return 0;
}

int getUsernameAndPasswdFromUrl(const char* target_url, char* u_and_p, int max_len) {
	int actual_len;
	char body[MAX_BUF_SIZE];
	memset((void*)body, 0x00, sizeof(body));
	if (getBodyFromUrl(target_url, body, sizeof(body))) {
		return -1;
	}
	char* at = NULL;
	at = strchr(body, '@');
	if (!at) {
		return -1;
	}
	actual_len = at - body;
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)u_and_p, (void*)body, actual_len);
	u_and_p[max_len - 1] = '\0';
	return 0;
}

int getUsernameFromUrl(const char* target_url, char* username, int max_len) {
	int actual_len;
	char u_and_p[MAX_BUF_SIZE];
	memset((void*)u_and_p, 0x00, sizeof(u_and_p));
	if (getUsernameAndPasswdFromUrl(target_url, u_and_p, sizeof(u_and_p))) {
		return -1;
	}
	char* cur_uandp = NULL;
	cur_uandp = strchr(u_and_p, ':');
	if (!cur_uandp) {
		return -1;
	}
	actual_len = cur_uandp - u_and_p;
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)username, (void*)u_and_p, actual_len);
	username[max_len - 1] = '\0';
	return 0;
}

int getPasswdFromUrl(const char* target_url, char* passwd, int max_len) {
	int actual_len;
	char u_and_p[MAX_BUF_SIZE] = {0};
	if (getUsernameAndPasswdFromUrl(target_url, u_and_p, sizeof(u_and_p))) {
		return -1;
	}
	char* cur_uandp = NULL;
	cur_uandp = strchr(u_and_p, ':');
	if (!cur_uandp) {
		return -1;
	}
	cur_uandp += 1;
	char* last = u_and_p + strlen(u_and_p);
	actual_len = last - cur_uandp;
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)passwd, (void*)cur_uandp, actual_len);
	passwd[max_len - 1] = '\0';
	return 0;
}

int getResPathFromUrl(const char* target_url, char* res_path, int max_len) {
	int actual_len;
	char hostname_start[MAX_BUF_SIZE] = {0};
	if (getHostnameStartFromUrl(target_url, hostname_start, sizeof(hostname_start))) {
		return -1;
	}
	const char* cur_hostname_start = strchr(hostname_start, '/');
	actual_len = strlen(cur_hostname_start);
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)res_path, (void*)cur_hostname_start, actual_len);
	res_path[max_len - 1] = '\0';
	return 0;
}

int getHostnameAndPortFromUrl(const char* target_url, char* h_and_p, int max_len) {
	int actual_len;
	char hostname_start[MAX_BUF_SIZE];
	memset((void*)hostname_start, 0x00, sizeof(hostname_start));
	if (getHostnameStartFromUrl(target_url, hostname_start, sizeof(hostname_start))) {
		return -1;
	}
	const char* res_path = strchr(hostname_start, '/');
	if (res_path != NULL) {
		actual_len = res_path - hostname_start;
	} else {
		actual_len = strlen(hostname_start);
	}
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)h_and_p, (void*)hostname_start, actual_len);
	h_and_p[max_len - 1] = '\0';
	return 0;
}

int getHostnameFromUrl(const char* target_url, char* hostname, int max_len) {
	int actual_len;
	char h_and_p[MAX_BUF_SIZE];
	memset((void*)h_and_p, 0x00, sizeof(h_and_p));
	if (getHostnameAndPortFromUrl(target_url, h_and_p, sizeof(h_and_p))) {
		return -1;
	}
	char* cur = NULL;
	cur = (char*)strchr(h_and_p, ':');
	if (!cur) {
		actual_len = strlen(h_and_p);
	} else {
		actual_len = cur - h_and_p;
	}
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)hostname, (void*)h_and_p, actual_len);
	hostname[max_len - 1] = '\0';
	return 0;
}

int getPortFromUrl(const char* target_url, int* port) {
	char d_and_p[MAX_BUF_SIZE];
	memset((void*)d_and_p, 0x00, sizeof(d_and_p));
	if (getHostnameAndPortFromUrl(target_url, d_and_p, sizeof(d_and_p))) {
		return -1;
	}
	char* cur = NULL;
	cur = strchr(d_and_p, ':');
	if (!cur) {
		return -1;
	} else {
		cur += 1;
	}
	*port = atoi(cur);
	return 0;
}

static bool regexMatch(const char* input, const char* regex) {
	regex_t regex_comp;
	int ret;
	ret = regcomp(&regex_comp, regex, REG_EXTENDED);
	if (ret != 0) {
		perror("regcomp");
		return false;
	}
	ret = regexec(&regex_comp, input, 0, NULL, 0);
	regfree(&regex_comp);
	if (ret != 0) {
		return false;
	}
	return true;
}
static int getIPFromDomain(const char* domain, char* ip, int max_len) {
	struct addrinfo hints, *res, *p;
	int status;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((status = getaddrinfo(domain, NULL, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return -1;
	}
	void* addr;
	p = res;
	if (p != NULL) {
		if (p->ai_family == AF_INET) {
			if (max_len < INET_ADDRSTRLEN) {
				fprintf(stderr, "Error : ip address buf length too small.\n");
				return -1;
			}
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
		} else {
			if (max_len < INET6_ADDRSTRLEN) {
				fprintf(stderr, "Error : ip address buf length too small.\n");
				return -1;
			}
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
		}
		inet_ntop(p->ai_family, addr, ip, max_len);
	} else {
		return -1;
	}
	freeaddrinfo(res);
	return 0;
}
#define IPv4_REG "((2(5[0-5]|[0-4][0-9]))|[0-1]?[0-9]{1,2})(\\.((2(5[0-5]|[0-4][0-9]))|[0-1]?[0-9]{1,2})){3}"
int getIPFromUrl(const char* target_url, char* ip, int max_len) {
	int actual_len;
	char hostname[MAX_BUF_SIZE];
	memset((void*)hostname, 0x00, sizeof(hostname));
	if (getHostnameFromUrl(target_url, hostname, sizeof(hostname))) {
		return -1;
	}
	bool ret = regexMatch(hostname, IPv4_REG);
	if (!ret) {
		if (getIPFromDomain(hostname, ip, INET6_ADDRSTRLEN)) {
			return -1;
		}
		return 0;
	}
	actual_len = strlen(hostname);
	if (actual_len > max_len - 1) {
		return -1;
	}
	memcpy((void*)ip, (void*)hostname, actual_len);
	ip[max_len - 1] = '\0';
	return 0;
}
