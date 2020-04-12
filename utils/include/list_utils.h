#ifndef __LIST_UTILS_H__
#define __LIST_UTILS_H__

#ifndef TRUE 
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef SUCESS
#define SUCCESS (0)
#endif

#ifndef ERROR
#define ERROR (-1)
#endif

typedef struct 
{
    void *obj;
    unsigned int len;
}item_t;

typedef struct _LIST_NODE
{
    item_t *item;
    struct _LIST_NODE *prev;
    struct _LIST_NODE *next;
}LIST_NODE;

typedef struct
{
    LIST_NODE *head;
    LIST_NODE *tail;
    unsigned int size;
}LIST;

#define FOR_EACH_NODE(node, list) for(LIST_NODE *node = list->head->next; node != list->tail; node=node->next)
#define LIST_SIZE(list) (list->size)

LIST* list_new();
int list_destroy(LIST *list);
int list_add(LIST *list, void *obj, unsigned int len);
int list_remove(LIST *list, void *obj);
#endif