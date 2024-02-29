#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *iterator, *safe;

    if (l == NULL)
        return;

    list_for_each_safe (iterator, safe, l) {
        element_t *entry = list_entry(iterator, element_t, list);

        list_del(iterator);
        q_release_element(entry);
    }

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!s || !head)
        return false;

    element_t *new_element = (element_t *) malloc(sizeof(element_t));
    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        q_release_element(new_element);
        return false;
    }

    list_add(&new_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!s || !head)
        return false;

    element_t *new_element = (element_t *) malloc(sizeof(element_t));
    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        q_release_element(new_element);
        return false;
    }

    list_add_tail(&new_element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_first_entry(head, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&entry->list);

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_last_entry(head, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(&entry->list);

    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int size = 0;
    struct list_head *iterator;

    list_for_each (iterator, head) {
        size++;
    }

    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *fast = head->next, *slow = head->next;

    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    element_t *entry = list_entry(slow, element_t, list);

    list_del(&entry->list);
    q_release_element(entry);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return false;
    }

    struct list_head *current, *safe;
    bool delete_flag = false;

    list_for_each_safe (current, safe, head) {
        element_t *current_entry = list_entry(current, element_t, list);
        element_t *next_entry = list_entry(safe, element_t, list);

        if (safe != head &&
            strcmp(current_entry->value, next_entry->value) == 0) {
            delete_flag = true;

            list_del(&current_entry->list);
            q_release_element(current_entry);
        } else {
            if (delete_flag) {
                delete_flag = false;

                list_del(&current_entry->list);
                q_release_element(current_entry);
            }
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *first = head->next, *second = first->next;

    while (first != head && second != head) {
        first->next = second->next;
        second->next->prev = first;
        second->next = first;
        second->prev = first->prev;
        first->prev->next = second;
        first->prev = second;

        first = first->next;
        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *current = head, *temp = NULL;

    do {
        temp = current->next;
        current->next = current->prev;
        current->prev = temp;

        current = temp;
    } while (current != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    int size = q_size(head);

    if (!head || list_empty(head) || size < k)
        return;

    struct list_head *prev = head;
    struct list_head *start = head->next, *current = head->next, *temp = NULL;

    do {
        int remain = k;
        size -= k;

        do {
            temp = current->next;
            current->next = current->prev;
            current->prev = temp;

            current = temp;
            remain--;
        } while (current != head && remain > 0);

        current->prev->prev = prev;
        prev->next = current->prev;
        start->next = current;
        prev = start;
        start = current;
    } while (size > k);
}

void merge(struct list_head *head,
           struct list_head *left,
           struct list_head *right,
           bool descend)
{
    struct list_head *temp = head;

    while (!list_empty(left) && !list_empty(right)) {
        element_t *left_entry = list_entry(left->next, element_t, list);
        element_t *right_entry = list_entry(right->next, element_t, list);

        if (descend ? strcmp(left_entry->value, right_entry->value) >= 0
                    : strcmp(left_entry->value, right_entry->value) <= 0) {
            list_move(left->next, temp);
        } else {
            list_move(right->next, temp);
        }

        temp = temp->next;
    }

    list_splice_tail(list_empty(left) ? right : left, temp->next);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    LIST_HEAD(left);
    LIST_HEAD(right);

    list_cut_position(&left, head, slow);
    list_splice_init(head, &right);

    q_sort(&left, descend);
    q_sort(&right, descend);

    merge(head, &left, &right, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *ptr = head->prev;
    element_t *current_entry = list_entry(ptr, element_t, list);
    char *current_min = current_entry->value;

    while (ptr != head && ptr->prev != head) {
        element_t *entry = list_entry(ptr->prev, element_t, list);

        if (strcmp(entry->value, current_min) <= 0) {
            current_min = entry->value;
            ptr = ptr->prev;
        } else {
            list_del(&entry->list);
            q_release_element(entry);
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *ptr = head->prev;
    element_t *current_entry = list_entry(ptr, element_t, list);
    char *current_max = current_entry->value;

    while (ptr != head && ptr->prev != head) {
        element_t *entry = list_entry(ptr->prev, element_t, list);

        if (strcmp(entry->value, current_max) >= 0) {
            current_max = entry->value;
            ptr = ptr->prev;
        } else {
            list_del(&entry->list);
            q_release_element(entry);
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *node, *safe;
    queue_contex_t *entry = list_entry(head->next, queue_contex_t, chain);
    int size = entry->size;

    list_for_each_safe (node, safe, head) {
        if (node == head->next)
            continue;

        queue_contex_t *current_entry = list_entry(node, queue_contex_t, chain);

        size += current_entry->size;
        list_splice_init(current_entry->q, entry->q);
    }

    q_sort(entry->q, descend);

    return size;
}