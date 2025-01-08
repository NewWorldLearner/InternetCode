# 记录网络编程的学习

## hello_server程序编写

服务器端：
// 第1步，获取socket:   int socket(int domain, int type, int protocol);
// 第2步，将socket和网络地址绑定起来:   int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
// 第3步，设置最大监听数:   int listen(int sockfd, int backlog);
// 第4步，等待连接请求, 没有连接请求时accept将会阻塞    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// 第5步，向socket写入消息

客户端：
// 第1步，生成socket    int socket(int domain, int type, int protocol);
// 第2步，连接服务器    int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
// 第3步，从socket中读取消息

服务端网络地址的初始化：
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

客户端网络地址的初始化：
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));