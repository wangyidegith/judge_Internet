#include <string.h>
#include <stdbool.h>

#include "m_http/m_http.h"

#include "judge_Internet.h"


bool judge_Internet(char* proxy_url) {
	const char* target_url = INTERNET_URL;
	char response[HTTP_RECV_BUF_SIZE] = "\0";
	if (proxy_url == NULL) {
		if (httpGet(target_url, response)) {
			return false;
		}
	} else {
		if (httpGetOverProxy(target_url, response, proxy_url)) {
			return false;
		}
	}
	char message[HTTP_RECV_BUF_SIZE] = "\0";
	getMessageFromHTTPResponse(response, message);
	int ret = strncmp(message, CREDENTIALS, strlen(CREDENTIALS));
	if (ret) {
		return false;
	}
	return true;
}
