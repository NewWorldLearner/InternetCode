#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>

#define BUFSIZE 1024
#define EPOLLSIZE 50

/*
 *   客户端多发送几次消息之后，客户端就不能正确的接收到回声消息了，错误应该出现在服务器的什么地方？
 *   在客户端断开连接之后，这个服务器也会结束进程，这是为什么呢？和SIGPIPE信号有关吗？
 *   bug的解决和回送消息有关，那一行代码写错了
 */

void error_handling(char *message);

int main(int argc, char **argv)
{

    char buff[BUFSIZE] = {0};
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 第1步，生成服务器socket
    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        error_handling("socket() error");
    }

    // 第2步，将socket和服务器地址绑定起来
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("bind() error");
    }

    // 第3步，设置服务器最大监听数量
    if (listen(server_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    // 第5步，生成epoll例程（空间）
    int epoll_fd = epoll_create(EPOLLSIZE);
    struct epoll_event *epoll_events = malloc(EPOLLSIZE * sizeof(struct epoll_event));

    // 第6步，在epoll例程中添加服务器socket
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_sock;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event);

    // 第7步，通过epoll来监听服务器连接以及和客户端之间的数据发送
    while (1)
    {
        int event_count = epoll_wait(epoll_fd, epoll_events, EPOLLSIZE, -1);
        if (event_count == -1)
        {
            puts("epoll_wait() error");
            break;
        }

        for (int i = 0; i < event_count; i++)
        {
            if (epoll_events[i].data.fd == server_sock)
            {
                int addr_size = sizeof(struct sockaddr_in);
                struct sockaddr_in client_addr;
                int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
                // epoll默认是边缘触发
                event.events = EPOLLIN;
                event.data.fd = client_sock;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event);
                printf("connected client %d", client_sock);
            }
            else
            {
                int strlen = read(epoll_events[i].data.fd, buff, BUFSIZE);
                // 读取到的数据为0，说明客户端关闭了连接
                if (strlen == 0)
                {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL);
                    close(epoll_events[i].data.fd);
                    printf("closed client: %d", epoll_events[i].data.fd);
                }
                else
                {
                    // 这里曾经有个bug——write(epoll_events[i].data.fd, buff, BUFSIZE);
                    // bug的现象是客户端发送了消息，但是却无法收到回声消息，这是为什么呢？
                    // 我猜测应该是这样的，假设客户端发送"abc\n"，然后服务器回送"abc\n\0\0\0\0……"
                    // 客户端的接收缓冲区在读取完"abc\n"之后，然后第2次发送消息时，读取到的都是0字符，所有看起来就是客户端发送了消息，但没有收到回声
                    // 这里造成的另外一个结果是，客户端退出以后，服务器也跟着退出了，这个是怎么导致的呢？
                    write(epoll_events[i].data.fd, buff, strlen);
                }
            }
        }
    }
    close(server_sock);
    close(epoll_fd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}