#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <tcp_client.h>
#include <io_utils.h>
#include <ulog.h>

int main(int argc, char *argv[])
{
    in_addr_t addr = 0;
    addr = inet_addr(argv[1]);
    addr = ntohl(addr);
    printf("0x%08x\n", addr);
    printf("Server address:");

    char *buf = NULL;
    buf = malloc(1024);
    io_getline(buf, 1024);
    printf("%s\n", buf);
    free(buf);
}