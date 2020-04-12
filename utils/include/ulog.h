#ifndef __ULOG_H__
#define __ULOG_H__
#include <libgen.h>

typedef enum {
    LEVEL_INFO = 0,
    LEVEL_DEBUG,
    LEVEL_WARNING,
    LEVEL_ERROR,
}ULOG_LEVEL;

typedef struct
{
    char name[32];
    int log_level;
}ULOG_MODULE;

#define ULOG(module, level, fmt, args...) ulog(__FILE__, __LINE__, module, level, fmt, ##args)
#define UPRINT(fmt, args...)    printf("[%s][%d]"fmt"\n", basename(__FILE__), __LINE__, ##args)
#define UNIX_ERROR(func)    UPRINT(func" errno:%d\n", errno)

#define ULOG_MODULE_SET_LEVEL(module, level)    (module->log_level = level)

int ulog_init(int buff_size);
void ulog_deinit();
ULOG_MODULE *ulog_register();
void ulog_unregister();
int ulog_set_bufsize(int buf_size);
void ulog(char *file, int line, const ULOG_MODULE *module, unsigned int level, const char *fmt, ...);

#endif