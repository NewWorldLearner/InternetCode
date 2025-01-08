#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

void error_handling(char *message);

int main(int argc, char **argv)
{
    
    if (argc != 2)
    {
        printf("Usage: %s <addr>", argv[0]);
    }

    struct hostent *host;
    host = gethostbyname(argv[1]);
    if (!host)
    {
        error_handling("gethost error");
    }
    printf("Offical name: %s\n", host->h_name);
    int i = 0;
    for (i = 0; host->h_aliases[i]; i++)
    {
        printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
    }
    printf("Address type: %s\n", host->h_addrtype == AF_INET? "AF_INET": "AF_INET6");
    for (i = 0; host->h_addr_list[i]; i++)
    {
        printf("IP addr %d: %s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}