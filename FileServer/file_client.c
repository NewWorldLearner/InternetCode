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
    int sock;
    FILE *fp;

    char buff[BUFSIZE] = {0};
    int read_cnt;
    struct sockaddr_in server_addr;


    if (argc != 3)
    {
        fprintf(stderr, "Usage <IP> <port>\n", argv[0]);
        exit(1);
    }

    fp = fopen("recvfile.dat", "wb");
    // 第1步，生成socket
    sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        error_handling("socket() error");
    }

    // 第2步，连接服务器
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("connect() error");
    }

    // 第3步，从服务器读取消息
    // 这里曾经写错了，写成了while(read_cnt = read(sock, buff, BUFSIZE) != 0)
    while((read_cnt = read(sock, buff, BUFSIZE)) != 0)
    {
        write(1, buff, read_cnt);
        fwrite((void*)buff, 1, read_cnt, fp);
    }

    puts("Received file data");
    write(sock, "Thank you", 10);
    fclose(fp);
    close(sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}