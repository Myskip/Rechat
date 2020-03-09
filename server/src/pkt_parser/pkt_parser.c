#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "pkt_parser.h"
#include "pkt.h"

void *parse_pkt(void *p)
{
    printf("begin parse pkt\n");
    PKT_PARSER *parser = (PKT_PARSER *) p;
    readn();
}

PKT_PARSER *pkt_parser_new(int client_fd)
{
    int ret = 0;
    if (client_fd < 0)
        return NULL;

    PKT_PARSER *parser = (PKT_PARSER *)malloc(sizeof(PKT_PARSER));
    if (NULL == parser)
        return NULL;

    ret = pthread_create(&parser->pid, NULL, parse_pkt, (void *)parser);
    if (0 != ret)
    {
        printf("[%s][%s] pthread_create failed, ret:%d\n", ret);
        goto exit;
    }

exit:
    pkt_parser_destroy(parser);
    return NULL;
}

void pkt_parser_destroy(PKT_PARSER *parser)
{
    int ret = 0;
    if(NULL == parser)
        return;
        
    ret = pthread_cancel(parser->pid);
    if(0 == ret)
    {
        printf("[%s][%s] pthread_destroy failed, ret:%d\n", ret);
    }

    close(parser->client_fd);
    free(parser);

}