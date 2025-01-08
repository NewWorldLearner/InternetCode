#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>

void error_handling(char *message);

int main(int argc, char **argv)
{
    // server_sock用于监听客户端的连接，client_sock用于和客户端的通信
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    char message[] = "Hello World!\n";
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    // 第1步，获取socket
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        error_handling("socket() error");
    }
    // 第2步，将socket和网络地址绑定起来
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("bind() error");
    }

    // 第3步，设置最大监听数
    if (listen(server_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    // 第4步，等待连接请求, 没有连接请求时accept将会阻塞
    client_addr_len = sizeof(client_addr);
    // accept将会返回一个socket用于和客户端通信
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock == -1)
    {
        error_handling("accept() error");
    }
    // 第5步，向socket写入消息
    write(client_sock, message, sizeof(message));
    close(server_addr);
    close(client_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}