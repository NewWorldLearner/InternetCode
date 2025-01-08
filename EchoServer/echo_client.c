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

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("connect() error");
    }
    else
    {
        puts("Connected successly\n");
    }

    // 第3步，向服务器发送消息和接收消息
    while (1)
    {
        fputs("Input message(Q to quit):", stdout);
        fgets(message, BUFSIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            break;
        }
        // // 这里需要注意的是，如果我们发送的数据太长，那么数据可能会被分成两个数据包来发送，结果可能就不会符合我们的预期
        // write(sock, message, strlen(message));
        // // 如果数据被分成两个数据包，那么服务器再收到第1个数据包时，可能会立即回送给客户端，而客户端立马读取数据包，结果不符合预期
        // int str_len = read(sock, message, BUFSIZE);
        int str_len = write(sock, message, strlen(message));
        int recv_len = 0;
        while (recv_len < str_len)
        {
            int recv_cnt = read(sock, &message[recv_len], BUFSIZE - 1);
            if (recv_cnt == -1)
            {
                error_handling("read() error");
            }
            recv_len += recv_cnt;
        }

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