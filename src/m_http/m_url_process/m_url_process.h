#define MAX_BUF_SIZE 500

int getProtoFromUrl(const char* target_url, char* proto, int max_len);
int getBodyFromUrl(const char* target_url, char* body, int max_len);
int getHostnameStartFromUrl(const char* target_url, char* hostname_start, int max_len);
int getUsernameAndPasswdFromUrl(const char* target_url, char* u_and_p, int max_len);
int getUsernameFromUrl(const char* target_url, char* username, int max_len);
int getPasswdFromUrl(const char* target_url, char* passwd, int max_len);
int getResPathFromUrl(const char* target_url, char* res_path, int max_len);
int getHostnameAndPortFromUrl(const char* target_url, char* h_and_p, int max_len);
int getHostnameFromUrl(const char* target_url, char* hostname, int max_len);
int getPortFromUrl(const char* target_url, int* port);
int getIPFromUrl(const char* target_url, char* ip, int max_len);
