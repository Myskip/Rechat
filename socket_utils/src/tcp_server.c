#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "tcp_server.h"

typedef struct server_info
{
    int server_fd;
    struct sockaddr_in addr_in;
    pthread_t connecting_process;
} server_info;

struct connect_info
{
    TCP_SERVER *server;
    int client_fd;
};

static void *on_client_connect_cb(void *p)
{
    struct connect_info *c_info = (struct connect_info *)p;
    if (NULL == p)
    {
        return NULL;
    }

    if (c_info->server->on_client_connect)
    {
        c_info->server->on_client_connect(c_info->client_fd);
    }

    free(p);
    return NULL;
}

static void *connecting_process(void *p)
{
    int client_fd = 0, ret = 0;
    TCP_SERVER *server = (TCP_SERVER *)p;
    server_info *info = (server_info *)server->handle;
    printf("accept routine start, server_fd:%d.\n", info->server_fd);

    while (1)
    {
        client_fd = accept(info->server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            printf("accept wrong client_fd:%d, errno:%d.\n", client_fd, errno);
            return NULL;
        }
        printf("accept client_fd:%d\n", client_fd);

        pthread_t pt;
        struct connect_info *c_info = malloc(sizeof(struct connect_info));
        if (NULL == c_info)
        {
            close(client_fd);
            continue;
        }

        ret = pthread_create(&pt, NULL, on_client_connect_cb, (void *)c_info);
        if (0 != ret)
        {
            printf("create accept calll back thread failed, ret:%d\n", ret);
            close(client_fd);
        }
    }

    return NULL;
}

/* create a tcp server, when client connected, ACCEPT_CB will be called. */
TCP_SERVER *tcp_server_create(char *addr, unsigned short port)
{
    int server_fd = 0;
    int ret = 0;
    server_info *info = NULL;

    /* check args */
    if (NULL == addr)
    {
        return NULL;
    }

    /* malloc server object */
    TCP_SERVER_HANDLE handle = malloc(sizeof(server_info));
    if (NULL == handle)
    {
        return NULL;
    }

    TCP_SERVER *server = malloc(sizeof(TCP_SERVER));
    if (NULL == server)
    {
        free(handle);
        return NULL;
    }

    /* create server socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        goto exit;
    }

    memset(handle, 0, sizeof(server_info));
    memset(server, 0, sizeof(TCP_SERVER));

    info = (server_info *)handle;
    info->addr_in.sin_family = AF_INET;
    info->addr_in.sin_addr.s_addr = htonl(inet_network(addr));
    info->addr_in.sin_port = port;
    info->server_fd = server_fd;
    server->handle = handle;

    ret = bind(server_fd, (struct sockaddr *)&info->addr_in, sizeof(struct sockaddr));
    if (ret < 0)
    {
        printf("[%s][%d] bind error, ret:%d, errno:%d\n", __FILE__, __LINE__, ret, errno);
        goto exit;
    }
    printf("bind at addr[0x%08x]\n", info->addr_in.sin_addr.s_addr);

    ret = listen(server_fd, 32);
    if (ret < 0)
    {
        printf("[%s][%d] listen error, ret:%d, errno:%d\n", __FILE__, __LINE__, ret, errno);
        goto exit;
    }
    printf("listening at socket[%d]\n", server_fd);

    ret = pthread_create(&info->connecting_process, NULL, connecting_process, (void *)server);
    if (ret != 0)
    {
        printf("accept thread create failed, ret:%d.\n", ret);
        goto exit;
    }

    return (TCP_SERVER_HANDLE)info;

exit:
    tcp_server_destroy((TCP_SERVER_HANDLE)info);
    printf("tcp server start failed.\n");
    return NULL;
}

void tcp_server_destroy(TCP_SERVER *server)
{
    int ret = 0;

    if (NULL == server)
    {
        return;
    }

    server_info *info = (server_info *)server->handle;
    if (info->connecting_process)
    {
        ret = pthread_cancel(info->connecting_process);
        if (0 != ret)
        {
            printf("tcp server pthread_cancel failed, ret:%d\n", ret);
        }
    }

    if (info->server_fd >= 0)
    {
        close(info->server_fd);
    }

    if (NULL != info)
    {
        free(info);
    }

    free(server);
}

int tcp_server_get_sockfd(TCP_SERVER *server)
{
    server_info *info = (server_info *)server->handle;
    return info->server_fd;
}
