#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{
    // server_sock用于监听客户端的连接，client_sock用于和客户端的通信
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    char message[BUFSIZE] = {0};
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
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("bind() error");
    }

    // 第3步，设置最大监听数
    if (listen(server_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    // 第4步，循环接受连接
    for (int i = 0; i< 5; i++)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock == -1)
        {
            error_handling("accept() error");
        }
        else
        {
            printf("Connected clinet %d\n", i + 1);
        }
        int str_len = 0;
        while ((str_len = read(client_sock, message, BUFSIZE)) != 0)
        {
            write(client_sock, message, str_len);
        }
        close(client_sock);
    }
    close(server_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}