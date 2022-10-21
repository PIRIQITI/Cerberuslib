#include "singly_linked.h"
#include <string.h>
#include <stdlib.h>

/* create operations */

static inline int default_compare(const void *a, const void *b)
{
    return strcmp((char *) a, (char *) b);
}

SLinked *SLinked_create(SLinked_cmp cmp)
{
    SLinked *list = calloc(1, sizeof(SLinked));
    check_mem(list);

    list->cmp_template = cmp == NULL ? default_compare : cmp;
    list->count = 0;

    return list;

error:
    return NULL;
}

static inline SLinkedNode *SLinkedNode_create()
{
    SLinkedNode *node = calloc(1, sizeof(SLinkedNode));
    check_mem(node);

    return node;

error:
    return NULL;
}

/* __insert operations */

// here we always __insert after *cur if it is NULL we make new node as the first element
// static inline int __insert(SLinked *list, SLinkedNode *cur, void *data) // this function got deprecated
// {
//     if (list->count == UINT32_MAX) {
//         log_err("list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);
//         goto error;
//     }

//     // create new node and assign given data to it
//     SLinkedNode *new_node = SLinkedNode_create();
//     check(new_node != NULL, "Failed to create new node.");
//     new_node->data = data;

//     if (cur) {
//         // get the next node and make it the next of new node
//         SLinkedNode *temp = cur->next;
//         new_node->next = temp;
//         cur->next = new_node;
//     } else if (!list->count) {
//         // means that we are __inserting the first element
//         list->first = new_node;
//     } else {
//         // means we are __inserting at the head
//         new_node->next = list->first;
//         list->first = new_node;
//     }
//     list->count++;

//     return 1;
  
// error:
//     return 0;
// }

int SLinked_push(SLinked *list, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count <= UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    SLinkedNode *new_node = SLinkedNode_create();
    check(new_node != NULL, "Failed to create new node.");

    new_node->data = data;
    new_node->next = NULL;

    // if list->first is NULL we skip this loop and __insert the first element
    SLinked_iter (list, cur) {
        if (!cur->next) {
            cur->next = new_node;
            list->count++;
            return CERB_OK;
        }
    }
    list->first = new_node;
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

int SLinked_unshift(SLinked *list, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count <= UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    SLinkedNode *new_node = SLinkedNode_create();
    check(new_node != NULL, "Failed to create new node.");

    new_node->data = data;
    new_node->next = list->first;
    list->first = new_node;
    list->count++;

    return CERB_OK;

error:
    return CERB_ERR;
}

static inline int in_list(SLinked *list, SLinkedNode *node) // checks if node is in given list
{
    SLinked_iter (list, cur) {
        if (cur == node) {
            return 1;
        }
    }

    return 0;
}

int SLinked_insert_after(SLinked *list, SLinkedNode *after, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(after != NULL, "Somehow got after that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count <= UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    if (in_list(list, after)) {
        SLinkedNode *new_node = SLinkedNode_create();
        check(new_node != NULL, "Failed to create new node.");

        new_node->data = data;
        new_node->next = after->next;
        after->next = new_node;
        list->count++;
        return CERB_OK;
    }

    log_err("Couldn't find node %p in list.", after);

error: // fall through
    return CERB_ERR;
}

int SLinked_insert_before(SLinked *list, SLinkedNode *before, void *data)
{
    check(list != NULL, "Somehow got list that is NULL.");
    check(before != NULL, "Somehow got before that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(list->count <= UINT32_MAX, "list has reached it's max length of %u AKA UINT32_MAX.", UINT32_MAX);

    // this can't be done with in_list() function so we are doing it this way
    // confirm that *after is indeed in this list
    SLinked_iter (list, cur) {
        if (cur->next == before) {                
            SLinkedNode *new_node = SLinkedNode_create();
            check(new_node != NULL, "Failed to create new node.");

            new_node->data = data;
            new_node->next = before;
            cur->next = new_node;
            list->count++;
            return CERB_OK;
        }
    }

    // if we are __inserting before the first element it is the same as unshifting
    if (list->first == before) {
        int rc = SLinked_unshift(list, data);
        check(rc != CERB_ERR, "Failed to insert before %p.", before);
        return rc;
    }

    log_err("Couldn't find node %p in list.", before);

error: // fall through
    return CERB_ERR;
}

/* remove operations */

// static inline void *__remove(SLinked *list, SLinkedNode *node) // this function got deprecated
// {
//     void *data = NULL;
//     SLinkedNode *remove_node = NULL;

//     if (!node) {
//         remove_node = list->first;
//         data = remove_node->data;
//         list->first = remove_node->next;
//         free(remove_node);
//     } else {
//         remove_node = node->next;
//         data = remove_node->data;
//         node->next = remove_node->next;
//         free(remove_node);
//     }
//     list->count--;

//     return data;
// }

void *SLinked_pop(SLinked *list)
{
    void *rc = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(list->count != 0, "Couldn't remove from an empty list.");

    if (list->count == 1) {
        // means we are removing the one and only element 
        rc = list->first->data;
        free(list->first);
        list->first = NULL;
        list->count = 0; // or --; as list gets empty
    } else {
        SLinked_iter (list, cur) {
            if (!cur->next->next) { // delete after cur
                rc = cur->next->data;
                free(cur->next);
                cur->next = NULL;
                list->count--;
            }
        }
    }

error: // fall through
    return rc;
}

void *SLinked_shift(SLinked *list)
{
    void *rc = NULL;

    check(list != NULL, "Somehow got list that is NULL.");

    if (list->first) {
        rc = list->first->data;
        SLinkedNode *first_node = list->first; // not to loose or insta free the pointer
        list->first = list->first->next;
        free(first_node);
        list->count--;
        return rc;
    }

    log_err("Couldn't shift from empty list.");

error: // fall through
    return rc;
}

// remove specific node from list
void *SLinked_remove(SLinked *list, SLinkedNode *node)
{
    void *rc = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(node != NULL, "Somehow got node that is NULL.");

    if (node == list->first) {
        rc = node->data;
        list->first = node->next;
        free(node);
        list->count--;
        return rc;
    }

    SLinked_iter (list, cur) {
        if (cur->next == node) {
            rc = node->data;
            cur->next = node->next;
            free(node);
            list->count--;
            return rc;
        }
    }

    log_err("Invalid node.");

error: // fall through
    return rc;
}

void *SLinked_remove_after(SLinked *list, SLinkedNode *after)
{
    void *rc = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(after != NULL, "Somehow got after that is NULL.");

    if (in_list(list, after) && after->next) {
        rc = after->next->data;
        SLinkedNode *after_next = after->next; // not to loose or insta free the pointer
        after->next = after->next->next;
        free(after_next);
        list->count--;
        return rc;
    }

    log_err("Invalid after node.");

error: // fall through
    return rc;
}

void *SLinked_remove_before(SLinked *list, SLinkedNode *before)
{
    void *rc = NULL;

    check(list != NULL, "Somehow got list that is NULL.");
    check(list->count != 0, "Couldn't remove from an empty list.");
    check(before != NULL, "Somehow got before that is NULL.");
    check(before != list->first, "Couldn't remove before the first element.");

    if (before == list->first->next) {
        // removing before the second element, so we are shifting
        rc = SLinked_shift(list);
        return rc;
    } else {
        // check to see if the *before is indeed in this list (can't be done with in_list() function)
        SLinked_iter (list, cur) {
            if (cur->next->next == before) {
                rc = cur->next->data;
                SLinkedNode *cur_next = cur->next;  // not to loose or insta free the pointer
                cur->next = before;
                free(cur_next);
                list->count--;
                return rc;
            }
        }
    }

    log_err("Invalid before node.");

error: // fall through
    return rc;
}

/* joining and splitting */

// this will extend list1 with elements of list2
int SLinked_join(SLinked **list1, SLinked **list2)
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

    SLinked_iter (*list1, cur) {
        if (!cur->next) {
            cur->next = (*list2)->first;
            (*list1)->count += (*list2)->count;
            break;
        }
    }

    free(*list2);
    *list2 = NULL;
    
    return CERB_OK;

error:
    return CERB_ERR;
}

// this will return new list splitted from **list, original list is changed via pointer (please provide an address)
SLinked *SLinked_split(SLinked **list, SLinkedNode *from_node, SLinkedNode *to_node)
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

    SLinked *new_list = NULL;
    uint32_t count = 0;

    if (from_node == NULL) {
        SLinked_iter (*list, cur) {

            count++;

            if (cur == to_node) {
                check(cur->next, "Doesn't make sense to split from first including last.");

                new_list = SLinked_create((*list)->cmp_template);
                check(new_list != NULL, "Couldn't create new list.");

                new_list->first = (*list)->first;
                (*list)->first = cur->next;
                cur->next = NULL;
                new_list->count = count;
                (*list)->count = (*list)->count - count;
                return new_list;
            }
        }
    } else if (to_node == NULL) {
        check(from_node != (*list)->first, "Doesn't make sense to split from first including last.");

        SLinked_iter (*list, cur) {

            count++;

            if (cur->next == from_node) {
                new_list = SLinked_create((*list)->cmp_template);
                check(new_list != NULL, "Couldn't create new list.");

                new_list->first = from_node;
                cur->next = NULL;
                new_list->count = (*list)->count - count;
                (*list)->count = count;
                return new_list;
            }
        }
    } else {

        if (from_node == (*list)->first) {
            new_list = SLinked_split(list, NULL, to_node);
            if (!new_list) goto error;
            return new_list;
        }
        
        // if we are splitting not from the first to not to the last
        SLinked_iter (*list, cur) {
            if (cur->next == from_node) {
                new_list = SLinked_create((*list)->cmp_template);
                check(new_list != NULL, "Couldn't create new list.");
                new_list->first = from_node;
                cur->next = to_node->next; // move link to to node's next element
                to_node->next = NULL; // set this to NULL as it becomes the last element of new list

                // make new loop for counting
                SLinkedNode *new_list_node = new_list->first;
                count++;
                while (new_list_node->next) {
                    new_list_node = new_list_node->next;
                    count++;
                }

                // set new_list->count to count coming from above while loop
                new_list->count = count;
                (*list)->count = (*list)->count - count;

                return new_list;
            }
        }
    }

error:
    return NULL;
}

/* clear and destroy operations */

int SLinked_clear(SLinked *list)
{
    check(list != NULL, "Somehow got list that is NULL.");

    SLinked_iter (list, cur) {
        free(cur->data);
        cur->data = NULL;
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

int SLinked_destroy(SLinked **list) // pass by reference to make it NULL, so no access after destroyed
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");
    
    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    SLinkedNode *cur_node = (*list)->first;
    SLinkedNode *next_node = NULL;
    
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

int SLinked_clear_destroy(SLinked **list) // pass by reference to make it NULL so no access after destroyed
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    SLinkedNode *cur_node = (*list)->first;
    SLinkedNode *next_node = NULL; // pottential bug here
    
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

int SLinked_free_complex_data(SLinked **list, free_func handler_func)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    if (!(*list)->count) goto empty_list; // if list is empty skip the loop

    SLinkedNode *cur_node = (*list)->first;
    SLinkedNode *next_node = NULL; // pottential bug here
    
    for (next_node = cur_node->next; next_node != NULL; next_node = cur_node->next) {
        handler_func(cur_node->data);
        free(cur_node);
        cur_node = next_node;
    }
    handler_func(cur_node->data);
    free(cur_node); // free the last one and we are done

empty_list: // fall through
    free(*list);
    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int SLinked_free_list(SLinked **list)
{
    check(list != NULL, "Somehow got an address of the list that is NULL.");
    check(*list != NULL, "Somehow got list that is NULL.");

    SLinked_iter (*list, cur) {
        free(cur);
    }

    free(*list);

    *list = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

/* sorted insertion */

int SLinked_sorted_insert(SLinked *list, void *data) // see if you can optimise insert before
{
    int rc = -1;

    check(list != NULL, "Somehow got list that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");

    // this function is optimisable by a lot but if i do
    // code becomes very ugly and hard to understand so i'll leave it as is
    
    SLinked_iter (list, cur) {
        if (list->cmp_template(cur->data, data) > 0) {
            rc = SLinked_insert_before(list, cur, data);
            check(rc != CERB_ERR, "Failed to sorted_insert.");
            return rc;
        }
    }
    // if all insert_before attampts fail it means that we have to insert at the end
    rc = SLinked_push(list, data);
    check(rc, "Failed to sorted_insert.");

error:
    return rc;
}