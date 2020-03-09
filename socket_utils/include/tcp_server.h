#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

typedef void (*ON_CLIENT_CONNECT)(int client_fd);
typedef void* TCP_SERVER_HANDLE;

typedef struct tcp_server
{
    TCP_SERVER_HANDLE handle;
    ON_CLIENT_CONNECT on_client_connect;
}TCP_SERVER;

TCP_SERVER* tcp_server_create(char *addr, unsigned short port);
void tcp_server_destroy(TCP_SERVER *server);
int tcp_server_get_sockfd(TCP_SERVER *server);
#endif