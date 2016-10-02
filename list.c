#include "list.h"

void list_init(list_t *list);
void list_add(list_t *pre, list_t *cur);
list_t *list_remove(list_t *cur);

void list_init(list_t *list)
{
        list->prev = list;
        list->next = list;
}


void list_add(list_t *pre, list_t *cur)
{
        cur->prev = pre;
        cur->next = pre->next;

        pre->next->prev = cur;
        pre->next = cur;
}


list_t *list_remove(list_t *cur)
{
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        cur->next = cur->prev = cur;
        list_init(cur);
        return cur;
}


