#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

void error_handling(char *message);
char message[30] = {0};

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage <IP> <port>\n", argv[0]);
    }
    // 第1步，生成socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        error_handling("socket() error");
    }

    // 第2步，连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    // in_addr_t inet_addr(const char *cp); 将ip字符串转换为大端序下的整数
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("connect() error");
    }


    // 第3步，从socket中读取消息
    int read_len = 0;
    int index = 0;
    // 一次只读取一个字节
    while(read_len = read(sock, &message[index++], 1))
    {
        if (read_len == -1)
        {
            error_handling("read() error");
        }
    }

    printf("Message from server: %s\n", message);
    close(sock);
    return 0;

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}