/*
 * This source file is part of the liblcthw library. Part of this code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#ifndef A7E33D4E_E4E9_4637_8A99_8C8A1F279553
#define A7E33D4E_E4E9_4637_8A99_8C8A1F279553

#define CERB_OK 0
#define CERB_ERR -1

#include <stdint.h>
#include "dbg.h"

typedef int (*DLinked_cmp) (const void *data1, const void *data2);
typedef void (*free_func) (void *data);

typedef struct DLinkedNode {
    struct DLinkedNode *next;
    struct DLinkedNode *prev;
    void *data;
} DLinkedNode;

typedef struct DLinked {
    DLinkedNode *first;
    DLinkedNode *last;
    // this is for sorting (if you'll need) when creating list you can pass NULL and default cmp will be set
    DLinked_cmp cmp_template;
    uint32_t count;
} DLinked;

// create DLinked *list ( specify cmp if you need to sorted insert )
DLinked *DLinked_create(DLinked_cmp cmp); //
// push data in list
int DLinked_push(DLinked *list, void *data); //
// pop data and return
void *DLinked_pop(DLinked *list); //
// insert in the front
int DLinked_unshift(DLinked *list, void *data); //
// remove first element from list and return
void *DLinked_shift(DLinked *list); //
// remove DLinkedNode *node from list
void *DLinked_remove(DLinked *list, DLinkedNode *node); //
// insert node after DLinkedNode *node
int DLinked_insert_after(DLinked *list, DLinkedNode *after, void *data); //
// remove node after DLinkedNode *node
void *DLinked_remove_after(DLinked *list, DLinkedNode *node); //
// insert node before DLinkedNode *node
int DLinked_insert_before(DLinked *list, DLinkedNode *before, void *data); //
// remove node before DLinkedNode *node
void *DLinked_remove_before(DLinked *list, DLinkedNode *node); //
// join 2 lists
// provide references to them
int DLinked_join(DLinked **list1, DLinked **list2); //
// split DLinked **list from node to node provide reference to
// never pass nodes with wrong order as it leads to some strange. always pass from left to right
DLinked *DLinked_split(DLinked **list, DLinkedNode *from_node, DLinkedNode *to_node); //

// to clear list from data it contains
// only use these on basic data structures ( int char float ) etc.
int DLinked_clear(DLinked *list);
// to free just list and its nodes but not data it contains
// always pass a reference to make it NULL after freeing
int DLinked_destroy(DLinked **list);
// combined DLinked_clear and DLinked_destroy
// always pass a reference to make it NULL after freeing
int DLinked_clear_destroy(DLinked **list);

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
// free_func is a typedef. you have to write your own function which matches the type of free_func and pass that
int DLinked_free_complex_data(DLinked **list, free_func handler_func);

int DLinked_free_list(DLinked **list); // to free just list and its nodes, but not data it contains

// sorted insert in list ( SHOULD BE INSERTED WITH THIS FUNCTION ONLY, IF YOU WANT TO HAVE SORTED ARRAY )
int DLinked_sorted_insert(DLinked *list, void *data);

#define DLinked_get_count(list) list->count

// L is DLinked *list
// S is starting node ( first or last ) D is direction ( next or prev )
// C is current node's name you want it to have
#define DLinked_iter(L, S, D, C) DLinkedNode *_node = NULL; DLinkedNode *C = NULL;\
        for (C = _node = (L)->S; _node != NULL; _node = C = _node->D)

// list is DLinked *list start is starting node ( first or last ) dir is direction ( next or prev )
// _data is a function pointer returning anything that matches format ( %s %d %u ) etc.
// format is "%s", "%d" ...
#define DLinked_print(list, start, dir, _data, format) if (list) {\
            printf("\n[ ");\
            DLinked_iter (list, start, dir, cur) { printf((format " -> "), _data(cur->data)); }\
            printf("NULL ]\n");\
        } else {\
            log_err("Somehow got list that is NULL.");\
        }

// list is DLinked *list
// start is starting node ( first or last ) dir is direction ( next or prev )
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define DLinked_search(list, start, dir, cmp_func, to_find, found_node) SLinkedNode *found_node = NULL;\
        if (list) {\
            DLinked_iter (list, start, dir, cur) {\
                if (cmp_func(to_find, cur->data) == 0) {\
                    found_node = cur;\
                    break;\
                }\
            }\
        } else {\
            log_err("Somehow got list that is NULL.");\
        }

#endif /* A7E33D4E_E4E9_4637_8A99_8C8A1F279553 */