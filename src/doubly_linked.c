/*
 * This source file is part of the liblcthw library. Part of this code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#include "doubly_linked.h"
#include <string.h>
#include <stdlib.h>

static inline int default_compare(const void *a, const void *b)
{
    return strcmp((char *) a, (char *) b);
}

DLinked *DLinked_create(DLinked_cmp cmp)
{
    DLinked *list = calloc(1, sizeof(DLinked));
    check_mem(list);

    list->cmp_template = cmp == NULL ? default_compare : cmp;
    list->count = 0;

    return list;

error:
    return NULL;
}

static inline DLinkedNode *DLinkedNode_create()
{
    DLinkedNode *node = calloc(1, sizeof(DLinkedNode));
    check_mem(node);

    return node;

error:
    return NULL;
}

/* insert operations */

int DLinked_push(DLinked *list, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count < UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    DLinkedNode *new_node = DLinkedNode_create();
    check(new_node != NULL, "Failed to push in list.");
    new_node->data = data;

    if (!list->count) { // if we only have one element first and last should be the same
        list->first = new_node;
        list->last = new_node;
    } else {
        DLinkedNode *temp = list->last;
        list->last = new_node;
        new_node->prev = temp;
        temp->next = new_node;
    }
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

int DLinked_unshift(DLinked *list, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count < UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    DLinkedNode *new_node = DLinkedNode_create();
    check(new_node != NULL, "Failed to push in list.");
    new_node->data = data;

    if (!list->count) { // if we only have one element first and last should be the same
        list->first = new_node;
        list->last = new_node;
    } else {
        DLinkedNode *temp = list->first;
        list->first = new_node;
        new_node->next = temp;
        temp->prev = new_node;
    }
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

static inline int in_list(DLinked *list, DLinkedNode *node) // add start_node too
{
    DLinked_iter (list, first, next, cur) { // change first with start_node
        if (cur == node) {
            return 1;
        }
    }

    return 0;
}

int DLinked_insert_after(DLinked *list, DLinkedNode *after, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(after != NULL, "Somehow got after that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count < UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    if (after == list->last) {
        int rc = DLinked_push(list, data);
        check(rc != CERB_ERR, "Failed insert after %p", after);
        return rc;
    } else if (in_list(list, after)) {
        DLinkedNode *new_node = DLinkedNode_create();
        check(new_node != NULL, "Failed to insert after %p.", after);
        new_node->data = data;

        DLinkedNode *temp = after->next;
        after->next = new_node;
        new_node->next = temp;
        new_node->prev = after;
        temp->prev = new_node;
        list->count++;
        return CERB_OK;
    } else {
        log_err("Couldn't find node %p.", after);
    }

error: // fall through
    return CERB_ERR;
}

int DLinked_insert_before(DLinked *list, DLinkedNode *before, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(before != NULL, "Somehow got after that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count < UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    if (list->first == before) {
        int rc = DLinked_unshift(list, data);
        check(rc != CERB_ERR, "Failed to insert before %p", before);
        return rc;
    } else if (in_list(list, before)) {
        DLinkedNode *new_node = DLinkedNode_create();
        check(new_node != NULL, "Failed to insert before %p", before);
        new_node->data = data;

        DLinkedNode *temp = before->prev;
        before->prev = new_node;
        new_node->prev = temp;
        new_node->next = before;
        temp->next = new_node;
        list->count++;
        return CERB_OK;
    } else {
        log_err("Couldn't find node %p.", before);
    }

error: // fall through
    return CERB_ERR;
}

/* remove operations */

void *DLinked_pop(DLinked *list)
{
    void *data = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(list->count != 0, "Couldn't shift from an empty list.");

    data = list->last->data;

    if (list->count == 1) {
        free(list->last);
        list->first = NULL;
        list->last = NULL;
        list->count = 0; // or --; as count gets 0
    } else {
        DLinkedNode *new_last = list->last->prev;
        free(list->last);
        list->last = new_last;
        new_last->next = NULL;
        list->count--;
    }

error: // fall through
    return data;
}

void *DLinked_shift(DLinked *list)
{
    void *data = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(list->count != 0, "Couldn't shift from an empty list.");

    data = list->first->data;

    if (list->count == 1) {
        free(list->first);
        list->first = NULL;
        list->last = NULL;
        list->count = 0;
    } else {
        DLinkedNode *new_first = list->first->next;
        free(list->first);
        list->first = new_first;
        new_first->prev = NULL;
        list->count--;
    }

error: // fall through
    return data;
}

void *DLinked_remove(DLinked *list, DLinkedNode *node)
{
    void *data = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(node != NULL, "Somehow got node that is NULL.");
    
    if (node == list->first) {
        data = DLinked_shift(list);
    } else if (node == list->last) {
        data = DLinked_pop(list);
    } else if (in_list(list, node)) {
        data = node->data;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        free(node);
        list->count--;
    } else {
        log_err("Couldn't find node %p.", node);
    }

error: // fall through
    return data;
}

void *DLinked_remove_after(DLinked *list, DLinkedNode *after)
{
    void *data = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(after != NULL, "Somehow got after that is NULL.");
    check(after != list->last, "Couldn't remove after last element.");

    if (after == list->last->prev) {
        data = DLinked_pop(list);
    } else if (in_list(list, after)) {
        DLinkedNode *delete_node = after->next;
        after->next = delete_node->next;
        after->next->prev = after;

        data = delete_node->data;
        free(delete_node);
        list->count--;
    } else {
        log_err("Couldn't find node %p.", after);
    }

error: // fall through
    return data;
}

void *DLinked_remove_before(DLinked *list, DLinkedNode *before)
{
    void *data = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(before != NULL, "Couldn't remove after NULL.");
    check(before != list->first, "Couldn't remove before first element.");

    if (before == list->first->next) {
        data = DLinked_shift(list);
    } else if (in_list(list, before)) {
        DLinkedNode *delete_node = before->prev;
        before->prev = delete_node->prev;
        before->prev->next = before;

        data = delete_node->data;
        free(delete_node);
        list->count--;
    } else {
        log_err("Couldn't find node %p.", before);
    }


    // if (before == list->first->next) { // this code got deprecated here and in other functions too
    //     data = DLinked_shift(list);
    // } else {
    //     DLinked_iter (list, first, next, cur) {
    //         if (cur == before) {
    //             DLinkedNode *delete_node = cur->prev;
    //             cur->prev = delete_node->prev;
    //             cur->prev->next = cur;

    //             data = delete_node->data;
    //             free(delete_node);
    //             list->count--;
    //             break;
    //         }
    //     }
    // }

error: // fall through
    return data;
}

/* joining and splitting */

int DLinked_join(DLinked **list1, DLinked **list2)
{
    check(list1 != NULL, "Somehow got an address of the list1 that is NULL.");
    check(*list1 != NULL, "Somehow got list1 that is NULL.");
    check(list2 != NULL, "Somehow got an address of the list2 that is NULL.");
    check(*list2 != NULL, "Somehow got list2 that is NULL.");
    check((*list1)->count != 0, "Doesn't make sense to join to an empty list.");
    check((*list2)->count != 0, "Doesn't make sense to join an empty list.");
    check((*list1)->count + (*list2)->count <= UINT32_MAX,
    "Couldn't join lists as length exceeds %u AKA UINT32_MAX.", UINT32_MAX);
    check((*list1)->cmp_template == (*list2)->cmp_template, "Couldn't join lists of different cmp_templates");

    (*list1)->last->next = (*list2)->first;
    (*list2)->first->prev = (*list1)->last;
    (*list1)->last = (*list2)->last;
    (*list1)->count += (*list2)->count;

    free(*list2);
    *list2 = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

DLinked *DLinked_split(DLinked **list, DLinkedNode *from_node, DLinkedNode *to_node)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");
    check(!(from_node == NULL && to_node == NULL), "Invalid nodes.");

    if (!in_list(*list, from_node) && from_node != NULL) {
        log_err("Invalid from_node.");
        return NULL;
    } else if (!in_list(*list, to_node) && to_node != NULL) {
        log_err("Invalid to_node.");
        return NULL;
    }

    DLinked *new_list = NULL;
    uint32_t count = 0;

    if (from_node == NULL) {
        check(to_node->next, "Doesn't make sense to split from first including last.");

        new_list = DLinked_create((*list)->cmp_template);
        check(new_list != NULL, "Couldn't create new list.");

        new_list->first = (*list)->first;
        new_list->last = to_node;

        (*list)->first = to_node->next;
        (*list)->first->prev = NULL;
        to_node->next = NULL;

    } else if (to_node == NULL) {
        check(from_node != (*list)->first, "Doesn't make sense to split from first including last.");

        new_list = DLinked_create((*list)->cmp_template);
        check(new_list != NULL, "Couldn't create new list.");

        new_list->first = from_node;
        new_list->last = (*list)->last;

        (*list)->last = from_node->prev;
        (*list)->last->next = NULL;
        from_node->prev = NULL;

    } else {

        if (from_node == (*list)->first) {
            new_list = DLinked_split(list, NULL, to_node);
            if (!new_list) goto error;
            return new_list;
        }

        new_list = DLinked_create((*list)->cmp_template);
        check(new_list != NULL, "Couldn't create new list.");

        from_node->prev->next = to_node->next;
        to_node->next->prev = from_node->prev;
        from_node->prev = NULL;
        to_node->next = NULL;

        new_list->first = from_node;
        new_list->last = to_node;
    }

    DLinkedNode *count_node = new_list->first;
    while (count_node) {
        count_node = count_node->next;
        count++;
    }

    new_list->count = count;
    (*list)->count = (*list)->count - count;

    return new_list;

error:
    return NULL;
}

/* clear and destroy operations */

int DLinked_clear(DLinked *list)
{
    check(list != NULL, "Somehow got list that is NULL.");

    DLinked_iter (list, first, next, cur) {
        free(cur->data);
        cur->data = NULL;
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

int DLinked_destroy(DLinked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");
    
    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    DLinkedNode *cur_node = (*list)->first;
    DLinkedNode *next_node = NULL;
    
    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next) {
        free(cur_node);
        cur_node = next_node;
    }
    free(cur_node); // free the last one and we are done

empty_list: // fall through
    free(*list);
    *list = NULL;

    return CERB_OK;


error:
    return CERB_ERR;
}
int DLinked_clear_destroy(DLinked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    DLinkedNode *cur_node = (*list)->first;
    DLinkedNode *next_node = NULL;
    
    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next) {
        free(cur_node->data);
        free(cur_node);
        cur_node = next_node;
    }
    free(cur_node->data);
    free(cur_node); // free the last one and we are done

empty_list: // fall through
    free(*list);
    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;

}

int DLinked_free_complex_data(DLinked **list, free_func handler_func)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    DLinkedNode *first = (*list)->first;
    DLinkedNode *freeable = NULL;
    for (; first != NULL;) {
        freeable = first;
        first = first->next; 
        handler_func(freeable->data);
        free(freeable);
    }
    free(*(list));
    *(list) = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int DLinked_free_list(DLinked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    DLinked_iter (*list, first, next, cur) {
        free(cur);
    }

    free(*list);

    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

/* sorted insertion */

int DLinked_sorted_insert(DLinked *list, void *data)
{
    int rc = -1;

    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");

    DLinked_iter (list, first, next, cur) {
        if (list->cmp_template(cur->data, data) > 0) {
            rc = DLinked_insert_before(list, cur, data);
            check(rc != CERB_ERR, "Failed to sorted_insert.");
            return rc;
        }
    }
    // if all insert_before attampts fail it means that we have to insert at the end
    rc = DLinked_push(list, data);
    check(rc != CERB_ERR, "Failed to sorted_insert.");

error:
    return rc;
}