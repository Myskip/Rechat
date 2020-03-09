#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

#include "net_utils.h"

int readn(int fd, u8 *buf, u32 len)
{
    int ret = 0, len = 0, read_len = 0, remain_len = len;
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
            len = read(fd, buf, remain_len);
            if(len < 0)
            {
                printf("read error, len:%d, errno:%d\n", len, errno);
                continue;
            }
            read_len += len;
            remain_len -= len;
        }
        else
        {
            printf("select error, ret:%d, errno:%d\n", ret, errno);
            break;
        }
    }
    
    return read_len;
}