#ifndef D84C1D58_017D_4D7A_947A_C40713539E7F
#define D84C1D58_017D_4D7A_947A_C40713539E7F

#define CERB_OK 0
#define CERB_ERR -1

#include <stdint.h>
#include "dbg.h"

typedef int (*SLinked_cmp) (const void *data1, const void *data2);
typedef void (*free_func) (void *data);

typedef struct SLinkedNode {
    void *data;
    struct SLinkedNode *next;
} SLinkedNode;

typedef struct SLinked {
    SLinkedNode *first;
    // this is for sorting (if you'll need) when creating list you can pass NULL and default cmp will be set
    SLinked_cmp cmp_template;
    uint32_t count;
} SLinked;

// create SLinked *list ( specify cmp if you need to sorted insert )
SLinked *SLinked_create(SLinked_cmp cmp); //
// push data in list
int SLinked_push(SLinked *list, void *data); //
// pop data from list and return
void *SLinked_pop(SLinked *list); //
// insert data at first position in list
int SLinked_unshift(SLinked *list, void *data); //
// remove first entry from list and return
void *SLinked_shift(SLinked *list); //
// remove SLinkedNode *node from list and return
void *SLinked_remove(SLinked *list, SLinkedNode *node); //
// insert data after SLinkedNode *after
int SLinked_insert_after(SLinked *list, SLinkedNode *after, void *data); //
// remove data after SLinkedNode *node and return
void *SLinked_remove_after(SLinked *list, SLinkedNode *node); //
// insert data before SLinkedNode *before
int SLinked_insert_before(SLinked *list, SLinkedNode *before, void *data); //
// remove data before SLinkedNode *node and return
void *SLinked_remove_before(SLinked *list, SLinkedNode *node); //
// join 2 lists
// provide references to them
int SLinked_join(SLinked **list1, SLinked **list2); //
// split SLinked **list from node to node, provide reference to list
// never pass nodes with wrong order as it leads to some strange. always pass from left to right
SLinked *SLinked_split(SLinked **list, SLinkedNode *from_node, SLinkedNode *to_node);

// to clear list from data it contains
// only use these on basic data structures ( int char float ) etc.
int SLinked_clear(SLinked *list); //
// to free just list and its nodes but not data it contains
// always pass a reference to make it NULL after freeing
int SLinked_destroy(SLinked **list);
// combined DLinked_clear and DLinked_destroy
// always pass a reference to make it NULL after freeing
int SLinked_clear_destroy(SLinked **list);

// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
// free_func is a typedef. you have to write your own function which matches the type of free_func and pass that
int SLinked_free_complex_data(SLinked **list, free_func handler_func);

int SLinked_free_list(SLinked **list); // to free just list and its nodes, but not data it contains

// sorted insert in list ( SHOULD BE INSERTED WITH THIS FUNCTION ONLY, IF YOU WANT TO HAVE SORTED ARRAY )
int SLinked_sorted_insert(SLinked *list, void *data);

// L is SLinked *list
// C is current node's name you want it to have
#define SLinked_iter(L, C) SLinkedNode *_node = NULL; SLinkedNode *C = NULL;\
        for (C = _node = (L)->first; _node != NULL; _node = C = _node->next)

// list is SLinked *list
// _data is a function pointer returning anything that matches format ( %s %d %u ) etc.
// format is "%s", "%d" ...
#define SLinked_print(list, _data, _format)\
        if (list) {\
            printf("\n[ ");\
            SLinked_iter (list, cur) { printf((_format " -> "), _data(cur->data)); }\
            printf("NULL ]\n");\
        } else {\
            log_err("Somehow got list that is NULL.");\
        }

// list is SLinked *list
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define SLinked_search(list, cmp_func, to_find, found_node) SLinkedNode *found_node = NULL;\
        if (list) {\
            SLinked_iter (list, cur) { if (cmp_func(to_find, cur->data) == 0) {\
                    found_node = cur;\
                    break;\
                }\
            }\
        } else {\
            log_err("Somehow got list that is NULL.");\
        }

// reverse SLinked *list
#define SLinked_reverse(list)\
    if (list) {\
        if (list->count > 1) {\
            SLinkedNode *first = list->first;\
            SLinkedNode *second = first->next;\
            first->next = NULL; /* NULL out first->next as it will become last->next */\
            SLinkedNode *third = second->next;\
            for (; third != NULL; second->next = first, first = second, second = third, third = second->next){}\
            second->next = first;\
            list->first = second;\
        } else {\
            log_err("Couldn't reverse list of count < 2.");\
        }\
    } else {\
        log_err("Somehow got list that is NULL.");\
    }

#endif /* D84C1D58_017D_4D7A_947A_C40713539E7F */
