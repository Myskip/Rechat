#include <pthread.h>

typedef struct {
    int client_fd;
    pthread_t pid;
}PKT_PARSER;