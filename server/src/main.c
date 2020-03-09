#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tcp_server.h>
#include <errno.h>

void on_client_connect(int client_fd)
{
    char buf[1024] = {0};
    int len = 0;
    while(1)
    {
        len = read(client_fd, buf, 1024);
        if(len < 0)
        {
            printf("read error, client_fd:%d, len:%d, errno:%d\n", client_fd, len, errno);
            continue;
        }

        printf("%s\n", buf);
    }
}

void main()
{
    printf("Rechat Server start...\n");
    TCP_SERVER *server = tcp_server_create("127.0.0.0", 8080);
    server->on_client_connect = on_client_connect;
    while (1)
    {
        char c = getchar();
        if (c == 'q')
        {
            tcp_server_destroy(server);
            exit(1);
        }
    }
}