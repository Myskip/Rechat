#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>


#include <ulog.h>
#include "net_utils.h"

int readn(int fd, u8 *buf, u32 len)
{
    int ret = 0, rlen = 0, read_len = 0, remain_len = len;
    fd_set fset;
    FD_ZERO(&fset);
    FD_SET(fd, &fset);

    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (remain_len > 0)
    {
        ret = select(fd, &fset, NULL, NULL, &timeout);
        if(ret == 0)
        {
            continue;
        }
        else if (ret > 0)
        {
            rlen = read(fd, buf, remain_len);
            if(rlen < 0)
            {
                UNIX_ERROR("read");
                continue;
            }
            read_len += rlen;
            remain_len -= rlen;
        }
        else
        {
            UNIX_ERROR("select");
            break;
        }
    }
    
    return read_len;
}

int writen(int fd, u8 *buf, u32 len)
{
    int ret = 0, wlen = 0, write_len = 0, remain_len = len;
    fd_set fset;
    FD_ZERO(&fset);
    FD_SET(fd, &fset);

    struct timeval timeout = {0};
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    remain_len = len;
    while (remain_len > 0)
    {
        ret = select(fd + 1, NULL, &fset, NULL, &timeout);
        if(ret == 0)
        {
            continue;
        }
        else if(ret > 0)
        {
            wlen = write(fd, buf, remain_len);
            if(wlen < 0)
            {
                UNIX_ERROR("write");
                continue;
            }
            write_len += wlen;
            remain_len -= wlen;
        }
        else
        {
            UNIX_ERROR("select");
            break;
        }
    }

    return write_len;
}