// define
// can change
#define MAX_LOCAL_IP_COUNT 32
#define LISTEN_QUEUE 10
// can't change
#define IPv4_BUF_SIZE 16
#define IPv4_WITH_MASK_BUF_SIZE 32

// function
int createUdpSocket();
int createUdpBindSocket(const char* ip, int port);
int createUdpBroadcastSocket();
int getLocalIPv4WithMaskAddresses(char ips[][IPv4_WITH_MASK_BUF_SIZE], int* count);
int ipv4StrtoUint(const char* ip);
int getBrdcAddr(char* broadcast_ip, const char* ip, const char* subnetmask);
int getIPFromDomain(const char* domain, char* ip, int max_len);
int createListenSocket(const char* ip, unsigned short port);
int createConnectSocket(const char* server_ip, int server_port);
int makeSocketNonBlocking(int sockfd);
unsigned short getOneRandomPort();
ssize_t readn(const int sock_fd, char* buffer, const size_t n);
ssize_t writen(const int sock_fd, char* buffer, const size_t n);
int getPeerIpAndPortFromSocket(int peer_socket, char* peer_ip, int* peer_port);
int getSelfIPFromSocket(int sock_fd, char* ip);
unsigned short getSelfPortFromSocket(int sock_fd);
