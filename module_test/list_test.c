#include <stdio.h>

#include "../list.h"

#define container_of(addr)              entry_of(addr, struct N, list)

struct N {
        list_t  list;
        int     a;
};

static void rebuild_list(struct N n[])
{
        for (int i = 0; i < 10; i++) {
                n[i].a = i;
                list_init(&n[i].list);
                if (i != 0) {
                        list_add(&n[i - 1].list, &n[i].list);
                }
        }
}


int main(void)
{
        struct N n[10];
        list_t *cur, *head, *tmp;
        rebuild_list(n);

        printf("============= iterate over each list while ================\n");

        cur = head = &n[0].list;
        printf("head = %d\n", container_of(head)->a);
        for (cur = (head)->next; cur != (head); cur = cur->next) {
                printf("value = %d\n", container_of(cur)->a);
        }

        printf("============= test for list_for_each() ====================\n");

        cur = head = &n[0].list;
        printf("head = %d\n", container_of(head)->a);
        list_for_each(cur, head) {
                printf("value = %d\n", container_of(cur)->a);
        }

        printf("============= test for list_for_each_continue() ===========\n");

        head = &n[0].list;
        cur = &n[5].list;
        printf("head = %d\n", container_of(head)->a);
        printf("cur = %d\n", container_of(cur)->a);
        list_for_each_continue(cur, head) {
                printf("value = %d\n", container_of(cur)->a);
        }

        printf("============= test for list_for_each_safe =================\n");

        cur = head = &n[0].list;
        printf("head = %d\n", container_of(head)->a);
        list_for_each_safe(cur, tmp, head) {
                if ((container_of(cur)->a & 0x2) != 0) {
                        printf("delete %d\n", container_of(cur)->a);
                        list_remove(cur);
                } else {
                        printf("value = %d\n", container_of(cur)->a);
                }
        }

        printf("        --------        \n");

        cur = head = &n[0].list;
        printf("head = %d\n", container_of(head)->a);
        list_for_each(cur, head) {
                printf("value = %d\n", container_of(cur)->a);
        }

        printf("============= test for list_for_each_continue_safe ========\n");
        rebuild_list(n);

        head = &n[0].list;
        cur = &n[2].list;
        printf("head = %d\n", container_of(head)->a);
        list_for_each_continue_safe(cur, tmp, head) {
                if ((container_of(cur)->a & 0x2) != 0) {
                        printf("delete %d\n", container_of(cur)->a);
                        list_remove(cur);
                } else {
                        printf("value = %d\n", container_of(cur)->a);
                }
        }

        printf("        --------        \n");

        cur = head = &n[0].list;
        printf("head = %d\n", container_of(head)->a);
        list_for_each(cur, head) {
                printf("value = %d\n", container_of(cur)->a);
        }

        printf("============= end =========================================\n");
        rebuild_list(n);

        return 0;
}
