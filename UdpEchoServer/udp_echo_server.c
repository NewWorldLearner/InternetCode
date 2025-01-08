#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{

    char message[BUFSIZE] = {0};
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    // 第1步，生成socket
    int server_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (server_sock == -1)
    {
        error_handling("socket() error");
    }

    // 第2步，将socket和网络地址绑定起来
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("bind() error");
    }

    // 第3步，接收客户端发送过来的消息并发送消息给客户端
    while(1)
    {
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        int str_len = recvfrom(server_sock, message, BUFSIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        sendto(server_sock, message, str_len, 0, (struct sockaddr *)&client_addr, client_addr_len);
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