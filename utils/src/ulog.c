#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#include <basic_defs.h>
#include <ulog.h>


static u8 *m_buf;
static u32 m_buf_size = 0;
static sem_t m_sem;
static char *level_names[4] = {
    "INFO",
    "DEBUG",
    "WARNING",
    "ERROR"
};

int ulog_init(int buff_size)
{
    m_buf = (u8 *)malloc(buff_size);
    if (NULL == m_buf)
    {
        UPRINT("malloc failed\n");
        return -1;
    }
    int ret = sem_init(&m_sem, 0, 0);
    if(ret != 0)
    {
        UNIX_ERROR("sem_init");
        free(m_buf);
        return -1;
    }
    
    if(0 != sem_post(&m_sem))
    {
        UNIX_ERROR("sem_post");
        ulog_deinit();
        return -1;
    }
    m_buf_size = buff_size;

    return 0;
}

void ulog_deinit()
{
    if(0 != sem_destroy(&m_sem))
    {
        UNIX_ERROR("sem_destroy");
    }

    if(m_buf)
    {
        free(m_buf);
        m_buf = NULL;
        m_buf_size = 0;
    }
}

ULOG_MODULE *ulog_register(char *name)
{
    ULOG_MODULE *module = (ULOG_MODULE *)malloc(sizeof(ULOG_MODULE));
    if(NULL == module)
    {
        UPRINT("malloc failed\n");
        return NULL;
    }

    strncpy(module->name, name, 32);
    module->log_level = LEVEL_WARNING;

    return module;
}

void ulog_unregister(ULOG_MODULE *module)
{
    if(NULL == module)
        return;

    free(module);
}

int ulog_set_bufsize(int buf_size)
{
    if(buf_size == m_buf_size)
    {
        return 0;
    }

    m_buf = realloc(m_buf, buf_size);
    if(NULL == m_buf)
    {
        UPRINT("realloc failed\n");
        return -1;
    }
    m_buf_size = buf_size;
    return 0;
}

void ulog(char *file, int line, const ULOG_MODULE *module, unsigned int level, const char *fmt, ...)
{
    int len = 0;
    if(NULL == module)
    {
        return;
    }

    if(module->log_level > level)
        return;

    struct timespec timeout = {0};
    if(0 != sem_wait(&m_sem))
    {
        UNIX_ERROR("sem_wait");
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    len = snprintf(m_buf, m_buf_size, "[FILE][%s][LINE][%d]:", basename(file), line);
    vsnprintf(m_buf + len, m_buf_size - len, fmt, ap);
    va_end(ap);
    printf("%s\n", m_buf);
    if(0 != sem_post(&m_sem))
    {
        UNIX_ERROR("sem_post");
    }

    return;
}        
