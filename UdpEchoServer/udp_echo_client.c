#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{
    char message[BUFSIZE] = {0};
    if (argc != 3)
    {
        fprintf(stderr, "Usage <IP> <port>\n", argv[0]);
    }
    // 第1步，生成socket
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        error_handling("socket() error");
    }

    // 第2步，设定服务器端的地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    // {
    //     error_handling("bind() error");
    // }

    while (1)
    {
        fputs("Input message(Q to quit):", stdout);
        fgets(message, BUFSIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            break;
        }
        // sendto需要指明目标主机的地址，sendto会自动给客户端分配地址
        sendto(sock, message, BUFSIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        struct sockaddr_in from_addr;
        int from_addr_len = sizeof(from_addr);
        // recvfrom保存源主机的地址
        int str_len = recvfrom(sock, message, BUFSIZE, 0, (struct sockaddr*)&from_addr, &from_addr_len);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}