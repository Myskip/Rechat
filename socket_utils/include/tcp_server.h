#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "basic_defs.h"

#define TCP_SERVER_DEBUG (1)

typedef int (*ON_RECEIVE_FUNC)(int fd, char *buf, u32 len);
typedef int (*SEND_FUNC)(int fd, char *buf, u32 len);
typedef void (*ON_CLIENT_CONNECT)(int client_fd);
typedef void* TCP_SERVER_HANDLE;

typedef struct tcp_server
{
    TCP_SERVER_HANDLE handle;
    ON_CLIENT_CONNECT on_client_connect;
    ON_RECEIVE_FUNC on_receive;
}TCP_SERVER;

TCP_SERVER* tcp_server_create(char *addr, unsigned short port);
void tcp_server_destroy(TCP_SERVER *server);
int tcp_server_get_server_fd(TCP_SERVER *server);
int tcp_server_get_clients_num(TCP_SERVER *server);
int tcp_server_send(TCP_SERVER *server, int fd, char *buf, u32 len);
#endif