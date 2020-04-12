#include <stdlib.h>
#include <string.h>

#include "list_utils.h"

/* create a item */
static item_t *item_new(void *obj, unsigned int len)
{
    if(!obj)
        return NULL;

    item_t *item = (item_t *)malloc(sizeof(item_t));
    if(NULL == item)
        return NULL;

    item->obj = obj;
    item->len = len;
    return item;
}

/* destroy item */
static void item_destroy(item_t *item)
{
    if(item)
        free(item);
}

static LIST_NODE *list_node_new(item_t *item)
{
    LIST_NODE *node = (LIST_NODE *)malloc(sizeof(LIST_NODE));
    if(NULL == node)
        return NULL;

    node->item = item;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

static void list_node_destroy(LIST_NODE *node)
{
    if(!node)
        return;
    
    if(node->item)
        item_destroy(node->item);

    node->item = NULL;
    node->prev = NULL;
    node->next = NULL;
    free(node);
}

static int list_node_exist(LIST *list, LIST_NODE *node)
{
    FOR_EACH_NODE(_node, list)
    {
        if(_node == node)
            return TRUE;
    }

    return FALSE;
}

static int list_add_node(LIST *list, LIST_NODE *node)
{
    list->tail->prev->next = node;
    node->prev = list->tail->prev;
    node->next = list->tail;
    list->tail->prev = node;

    list->size++;

    return SUCCESS;
}

static int list_remove_node(LIST *list, LIST_NODE *node)
{
    if(!list_node_exist(list, node))
        return ERROR;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    list->size--;

    list_node_destroy(node);
    return SUCCESS;
}

LIST* list_new()
{
    LIST *list = malloc(sizeof(LIST));
    if(!list)
        return NULL;

    list->head = (LIST_NODE *)malloc(sizeof(LIST_NODE));
    if(!list->head)
    {
        free(list);
        return NULL;
    }

    list->tail = (LIST_NODE *)malloc(sizeof(LIST_NODE));
    if(!list->tail)
    {
        free(list->head);
        free(list);
        return NULL;
    }

    list->head->prev = NULL;
    list->head->next = list->tail;
    list->tail->prev = list->head;
    list->tail->next = NULL;
    list->size = 0;
    return list;
}

int list_destroy(LIST *list)
{
    if(!list)
        return ERROR;

    FOR_EACH_NODE(node, list)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if(list_remove(list, node))
            return ERROR;
        list_node_destroy(node);
    }

    if(list->head)
        free(list->head);
    
    if(list->tail)
        free(list->tail);

    free(list);

    return SUCCESS;
}

static int list_find(LIST *list, void *obj)
{
    int find = FALSE;
    if(!list || !obj)
        return FALSE;

    FOR_EACH_NODE(node, list)
    {
        if(node->item->obj == obj)
        {
           find = TRUE; 
           break;
        }
    }

    return find;
}

int list_add(LIST *list, void *obj, unsigned int len)
{
    item_t *item = NULL;
    LIST_NODE *node = NULL;

    item = item_new(obj, len);
    if(!item)
        goto exit;

    node = list_node_new(item);
    if(!node)
        goto exit;

    return list_add_node(list, node);

exit:
    if(item)
        item_destroy(item);

    if(node)
        list_node_destroy(node);

    return ERROR;
}

int list_remove(LIST *list, void *obj)
{
    FOR_EACH_NODE(node, list)
    {
        if(node->item->obj == obj)
        {
            return list_remove_node(list, node);
        }
    }

    return ERROR;
}

