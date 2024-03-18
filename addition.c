#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "addition.h"

void q_shuffle(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;

    int size = q_size(head);
    struct list_head *current = head->prev;

    while (size && current != head) {
        int index = rand() % (size--);
        struct list_head *target = head->next;

        while (index--)
            target = target->next;

        if (current != target) {
            struct list_head *temp = target->prev;
            list_move(target, current);
            list_move(current, temp);
            current = target->prev;
        } else {
            current = current->prev;
        }
    }
}