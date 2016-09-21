#ifndef __LIST_H
#define __LIST_H

#include <string.h>
#include <stddef.h>

#define entry_of(ptr, type, member)     ((type *)(ptr + offsetof(type, member)))

struct list {
        struct list     *next;
        struct list     *prev;
};
typedef struct list     list_t;


extern void list_init(list_t *list);
extern void list_add(list_t *pre, list_t *cur);
extern list_t *list_remove(list_t *cur);


#define list_for_each(cur, head)                                              \
        for (cur = (head)->next; cur != (head); cur = cur->next)

#define list_for_each_safe(cur, tmp, head)                                    \
        for (cur = (head)->next, tmp = (cur)->next;                           \
             cur != (head);                                                   \
             cur = tmp, tmp = (tmp)->next)

#define list_for_each_continue(cur, head)                                     \
        for (cur = (cur)->next; cur != (head); cur = (cur)->next)

#define list_for_each_continue_safe(cur, tmp, head)                           \
        for (cur = (cur)->next, tmp = (cur)->next;                            \
             cur != (head);                                                   \
             cur = tmp, tmp = (tmp)->next)


#endif
