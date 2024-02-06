#define HTTP_SEND_BUF_SIZE 1024
#define HTTP_RECV_BUF_SIZE 1024

int httpGet(const char* target_url, char* response);
int httpGetOverProxy(const char* target_url, char* response, const char* proxy_url);
void getContentLengthFromHTTPResponse(const char* response, int* content_length);
void getMessageFromHTTPResponse(const char* response, char* message);
