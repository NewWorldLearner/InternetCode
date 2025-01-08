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
    char buff[BUFSIZE] = {0};
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    // 第1步，生成socket
    int server_sock = socket(PF_INET, SOCK_STREAM, 0);

    // 第2步，将服务器地址和socket绑定起来
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 第3步，设置最大监听数量
    listen(server_sock, 5);

    // 第4步，进入监听状态
    int server_addr_len = sizeof(server_addr);
    int client_sock = accept(server_sock, (struct sockaddr*)&server_addr, &server_addr_len);

    FILE * fp = fopen("file_server.c", "rb");
    // 第5步，向客户端发送信息
    while (1)
    {
        // 从文件中读取内容到缓冲区中
        int read_cnt = fread(buff, 1, BUFSIZE, fp);
        // read_cnt小于BUFSIZE时，说明是最后一次从文件中读取
        if (read_cnt < BUFSIZE)
        {
            write(client_sock, buff, read_cnt);
            write(1, buff, read_cnt);
            break;
        }
        write(client_sock, buff, BUFSIZE);
        write(1, buff, read_cnt);
    }

    // 第6步，半关闭，关闭写
    shutdown(client_sock, SHUT_WR);
    read(client_sock, buff, BUFSIZE);
    printf("Message from client: %s", buff);

    fclose(fp);
    close(client_sock);
    close(server_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}