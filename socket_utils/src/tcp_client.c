#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int tcp_connet(char *addr, unsigned short port)
{
    struct sockaddr_in addr_in = {0};
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl(inet_network(addr));
    addr_in.sin_port = port;

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    int ret = connect(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in));
    if(ret < 0)
    {
        printf("connet addr:[0x%08x] failed, ret:%d, errno:%d.\n", addr_in.sin_addr.s_addr, ret, errno);
        return -1;
    }
    else
    {
        printf("connet addr:[0x%08x] success\n", addr_in.sin_addr.s_addr);
        return sock_fd;
    }
}

