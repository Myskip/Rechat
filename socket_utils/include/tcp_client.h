#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "basic_defs.h"

typedef int (*ON_RECEIVE_FUNC)(char *buf, u32 len);
typedef void * TCP_CLIENT_HANDLE;
typedef struct{
    TCP_CLIENT_HANDLE handle;
    ON_RECEIVE_FUNC on_receive;
}TCP_CLIENT;

TCP_CLIENT *tcp_client_create(char *addr, u32 port, ON_RECEIVE_FUNC on_receive);
void tcp_client_destroy(TCP_CLIENT *client);
int tcp_client_connet(TCP_CLIENT *client, char *addr, unsigned short port);
int tcp_client_send(TCP_CLIENT *client, char *buf, u32 len);
#endif