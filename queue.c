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
    struct list_head *q = malloc(sizeof(struct list_head));
    // check if malloc success
    if (q)
        INIT_LIST_HEAD(q);

    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;

    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }

    list_add(&ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;

    ele->value = strdup(s);
    if (!ele->value) {
        free(ele);
        return false;
    }

    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ele = list_first_entry(head, element_t, list);
    list_del(head->next);

    if (sp && bufsize) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *ele = list_last_entry(head, element_t, list);
    list_del(head->prev);

    if (sp && bufsize) {
        strncpy(sp, ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return NULL;

    struct list_head *fast, *slow;
    for (fast = slow = head->next; fast != head && fast->next != head;
         slow = slow->next, fast = fast->next->next)
        ;
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    struct list_head *node, *safe;
    bool isdup = false;
    list_for_each_safe (node, safe, head) {
        element_t *ele = list_entry(node, element_t, list);
        bool match =
            node->next != head &&
            !strcmp(ele->value, list_entry(node->next, element_t, list)->value);
        if (match || isdup) {
            list_del(node);
            q_release_element(ele);
        }
        isdup = match;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;

    struct list_head *node;
    for (node = head->next; node != head && node->next != head;
         node = node->next) {
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    for (node = head->next->next, safe = node->next; node != head;
         node = safe, safe = node->next)
        list_move(node, head);
}

struct list_head *mergeTwoLists(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; l1 && l2; *node = (*node)->next) {
        node = strcmp(list_entry(l1, element_t, list)->value,
                      list_entry(l2, element_t, list)->value) < 0
                   ? &l1
                   : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);
    return head;
}

struct list_head *mergeSort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *fast, *slow;
    for (fast = slow = head; fast && fast->next;
         slow = slow->next, fast = fast->next->next)
        ;

    slow->prev->next = NULL;

    struct list_head *left = mergeSort(head);
    struct list_head *right = mergeSort(slow);
    return mergeTwoLists(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = mergeSort(head->next);

    struct list_head *cur = head, *next = head->next;
    while (next) {
        next->prev = cur;
        cur = next;
        next = next->next;
    }
    cur->next = head;
    head->prev = cur;
}
