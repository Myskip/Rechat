#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

#include <ulog.h>
#include <net_utils.h>
#include "tcp_client.h"

typedef struct
{
    int fd;
    pthread_t recv_thread;
} client_info;

#define FOREVER while (1)
#define CLIENT_INFO(client) ((client_info *)(client->handle))
#define CLIENT_FD(client) (CLIENT_INFO(client)->fd)
#define CLIENT_INFO_INIT(client, fd) do{\
    memset(client->handle, 0, sizeof(client_info));\
    CLIENT_FD(client) = fd;\
}while(0)

static void *_process_receive(void *p)
{
    char buf[1024] = {0};
    TCP_CLIENT *client = (TCP_CLIENT *)p;

    FOREVER
    {
        int len = read(CLIENT_FD(client), buf, 1024);
        if (len <= 0)
            continue;
        client->on_receive(buf, len);
    }
}

TCP_CLIENT *tcp_client_create(char *addr, u32 port, ON_RECEIVE_FUNC on_receive)
{
    TCP_CLIENT *client = (TCP_CLIENT *)malloc(sizeof(TCP_CLIENT));
    if (NULL == client)
        return NULL;

    client->handle = malloc(sizeof(client_info));
    if (NULL == client->handle)
    {
        free(client);
        return NULL;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        free(client->handle);
        free(client);
        UPRINT("[%s][%d] fd error, fd:%d", __FILE__, __LINE__, fd);
        return NULL;
    }

    CLIENT_INFO_INIT(client, fd);
    client->on_receive = on_receive;

    return client;
}

void tcp_client_destroy(TCP_CLIENT *client)
{
    if (CLIENT_INFO(client)->recv_thread)
        pthread_cancel(CLIENT_INFO(client)->recv_thread);

    if (client->handle)
        free(client->handle);

    if (client)
        free(client);
}

int tcp_client_connet(TCP_CLIENT *client, char *addr, unsigned short port)
{
    struct sockaddr_in addr_in = {0};
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl(inet_network(addr));
    addr_in.sin_port = port;

    int ret = connect(CLIENT_FD(client), (struct sockaddr *)&addr_in, sizeof(addr_in));
    if (ret < 0)
    {
        UNIX_ERROR("connect");
        UPRINT("addr:[0x%08x]", addr_in.sin_addr.s_addr);
        return -1;
    }

    ret = pthread_create(&(CLIENT_INFO(client)->recv_thread), NULL, _process_receive, (void *)client);
    if (ret != 0)
    {
        UPRINT("pthread_create error, ret:%d", ret);
        return -1;
    }

    return 0;
}

int tcp_client_send(TCP_CLIENT *client, char *buf, u32 len)
{
    return writen(CLIENT_FD(client), buf, len);
}

