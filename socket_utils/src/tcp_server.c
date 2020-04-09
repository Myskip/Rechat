#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <list_utils.h>
#include <net_utils.h>
#include "tcp_server.h"

#define TCP_SERVER_LOG(fmt, args...) \
    do                               \
    {                                \
        if (TCP_SERVER_DEBUG == 1)   \
            printf("FILE[%s]LINE[%d]" fmt, basename(__FILE__), __LINE__, ##args);     \
    } while (0)

#define FOREVER while (1)
#define SERVER_INFO(server) ((server_info *)server->handle)

typedef struct server_info
{
    int server_fd;
    struct sockaddr_in addr_in;
    pthread_t connecting_process;
    LIST *connection_list;
} server_info;

typedef struct connection_info
{
    TCP_SERVER *server;
    int client_fd;
    pthread_t receive_thread;
} connection_info;

static int _find_fd(TCP_SERVER *server, int fd)
{
    int find = FALSE;
    LIST *connection_list = SERVER_INFO(server)->connection_list;
    FOR_EACH_NODE(node, connection_list)
    {
        connection_info *connection = (connection_info *)node->item->obj;
        if(fd == connection->client_fd)
        {
            find = TRUE;
            break; 
        }
    }

    return find;
}

static void *_on_receive(void *p)
{
    char buf[1024] = {0};
    connection_info *connection = (connection_info *)p;

    FOREVER
    {
        int len = read(connection->client_fd, buf, 1024);
        if (len <= 0)
            continue;

        if (connection->server->on_receive)
            connection->server->on_receive(connection->client_fd, buf, len);
    }
}

static void *_on_client_connect(void *p)
{
    connection_info *connection = (connection_info *)p;
    if (NULL == p)
        return NULL;

    /* add client info to server`s list */
    LIST *connection_list = SERVER_INFO(connection->server)->connection_list;
    int ret = list_add(connection_list, connection, sizeof(connection_info));
    if (ret)
        TCP_SERVER_LOG("connection_list add client_fd[%d] failed\n", connection->client_fd);
    TCP_SERVER_LOG("connection_list add client_fd[%d] success\n", connection->client_fd);

    if(connection->server->on_client_connect)
        connection->server->on_client_connect(connection->client_fd);

    // TODO:recv & send thread
    ret = pthread_create(&connection->receive_thread, NULL, _on_receive, (void *)connection);
    if (ret)
        TCP_SERVER_LOG("create _on_receive thread error, fd:%d, ret:%d\n", connection->client_fd, ret);

    return NULL;
}

static connection_info *_create_connection_info(TCP_SERVER *server, int client_fd)
{
    connection_info *connection = malloc(sizeof(connection_info));
    if (NULL == connection)
        return NULL;
    memset(connection, 0, sizeof(connection_info));

    connection->server = server;
    connection->client_fd = client_fd;
    return connection;
}

static void *_process_connect_thread(void *p)
{
    int ret = 0;
    TCP_SERVER *server = (TCP_SERVER *)p;
    TCP_SERVER_LOG("accept routine start, server_fd:%d.\n", SERVER_INFO(server)->server_fd);

    while (1)
    {
        int client_fd = accept(SERVER_INFO(server)->server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            TCP_SERVER_LOG("accept error, client_fd:%d, errno:%d.\n", client_fd, errno);
            continue;
        }
        TCP_SERVER_LOG("accept client_fd:%d\n", client_fd);

        pthread_t pt;
        connection_info *connection = _create_connection_info(server, client_fd);
        if (!connection)
        {
            close(client_fd);
            continue;
        }

        ret = pthread_create(&pt, NULL, _on_client_connect, (void *)connection);
        if (0 != ret)
        {
            TCP_SERVER_LOG("create accept calll back thread failed, ret:%d\n", ret);
            close(client_fd);
        }
    }

    return NULL;
}

static void _close_connections(LIST *connection_list)
{
    int ret = 0;
    FOR_EACH_NODE(node, connection_list)
    {
        connection_info *connection = (connection_info *)node->item->obj;
        ret = pthread_cancel(connection->receive_thread);
        if(ret)
            TCP_SERVER_LOG("_close_connections fd[%d], pthread_cancal failed, ret:%d\n", connection->client_fd, ret);
        
        close(connection->client_fd);

        ret = list_remove(connection_list, connection);
        if(ret)
            TCP_SERVER_LOG("_close_connections fd[%d], list_remove error, ret:%d\n", connection->client_fd, ret);

        free(connection);
    }
}

static server_info *_malloc_server_info()
{
    server_info *info = malloc(sizeof(server_info));
    if (!info)
        return NULL;
    memset(info, 0, sizeof(server_info));

    info->connection_list = list_new();
    if (!info->connection_list)
    {
        free(info);
        return NULL;
    }

    return info;
}

static void _close_server_info(server_info *info)
{
    int ret = 0;

    if (info->connecting_process)
    {
        ret = pthread_cancel(info->connecting_process);
        if (ret)
            TCP_SERVER_LOG("tcp server pthread_cancel failed, ret:%d\n", ret);
    }

    if(info->server_fd >= 0)
        close(info->server_fd);

    if(info->connection_list)
        _close_connections(info->connection_list);

    ret = list_destroy(info->connection_list);
    if(ret)
        TCP_SERVER_LOG("_close_connections list_destroy error, ret:%d", ret);

    free(info);
}

static int _create_tcp_server_fd(char *addr, unsigned short port)
{
    int ret = 0, server_fd = -1;
    struct sockaddr_in addr_in = {0};
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl(inet_network(addr));
    addr_in.sin_port = port;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        goto exit;

    ret = bind(server_fd, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        TCP_SERVER_LOG("bind error, ret:%d, errno:%d\n", ret, errno);
        goto exit;
    }
    TCP_SERVER_LOG("bind at addr[0x%08x]\n", addr_in.sin_addr.s_addr);

    ret = listen(server_fd, 32);
    if (ret < 0)
    {
        TCP_SERVER_LOG("listen error, ret:%d, errno:%d\n", ret, errno);
        goto exit;
    }
    TCP_SERVER_LOG("listening at socket[%d]\n", server_fd);
    return server_fd;

exit:
    if(server_fd >= 0)
        close(server_fd);
    return -1;

}

/* create a tcp server, when client connected, ACCEPT_CB will be called. */
TCP_SERVER *tcp_server_create(char *addr, unsigned short port)
{
    int ret = 0;

    /* check args */
    if (!addr)
        return NULL;

    TCP_SERVER *server = malloc(sizeof(TCP_SERVER));
    if (!server)
        return NULL;
    memset(server, 0, sizeof(TCP_SERVER));

    server->handle = (TCP_SERVER_HANDLE)_malloc_server_info();
    if (!server->handle)
        goto exit;

    /* create server socket */
    int server_fd = _create_tcp_server_fd(addr, port);
    if(server_fd < 0)
        goto exit;
    SERVER_INFO(server)->server_fd = server_fd;

    ret = pthread_create(&SERVER_INFO(server)->connecting_process, NULL, _process_connect_thread, (void *)server);
    if (ret != 0)
    {
        TCP_SERVER_LOG("accept thread create failed, ret:%d.\n", ret);
        goto exit;
    }

    return (TCP_SERVER *)server;

exit:
    tcp_server_destroy(server);
    TCP_SERVER_LOG("tcp server start failed.\n");
    return NULL;
}

void tcp_server_destroy(TCP_SERVER *server)
{
    int ret = 0;
    server_info *info = NULL;
    if (!server)
        return;

    info = SERVER_INFO(server);
    if (info)
    {
        _close_server_info(info);
    }

    free(server);
}

int tcp_server_get_server_fd(TCP_SERVER *server)
{
    if (NULL == server)
        return -1;

    return SERVER_INFO(server)->server_fd;
}

int tcp_server_get_clients_num(TCP_SERVER *server)
{
    if (!server)
        return -1;

    return LIST_SIZE(SERVER_INFO(server)->connection_list);
}

int tcp_server_send(TCP_SERVER *server, int fd, char *buf, u32 len)
{
    if (!server || fd < 0 || !buf)
        return -1;

    if(!_find_fd(server, fd))
        return -1;
    
    return writen(fd, buf, len);
}