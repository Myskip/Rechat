#include <stdio.h>


int io_getline(char *buf, int len)
{
    char c = 0;
    int i = 0;

    if(NULL == buf)
    {
        return -1;
    }

    while(1)
    {
        c = getchar();
        if(c == '\n')
        {
            buf[i] = '\0';
            break;
        }
        else
        {
            buf[i] = c;
        }

        i++;
        if(i == len)
        {
            buf[i] = '\0';
            break;
        }
    }
    return i;
}