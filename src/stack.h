#ifndef DA30B4E4_4771_452B_ABA5_8E814ED70200
#define DA30B4E4_4771_452B_ABA5_8E814ED70200

#include "doubly_linked.h"

#define Stack DLinked

// get how many elements stack has
#define Stack_get_count(stack) DLinked_get_count(stack)

// create a stack
#define Stack_create(cmp_callback) DLinked_create(cmp_callback)
// push void *data in stack
#define Stack_push(stack, data) DLinked_push(stack, data)
// pop data from stack and return
#define Stack_pop(stack) DLinked_pop(stack)
// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
#define Stack_free_complex_data(stack, handler_func) DLinked_free_complex_data(stack, handler_func)

// iterate through stack
// stack is Stack *, cur is current node's name you specify
#define Stack_iter(stack, cur) DLinked_iter(stack, last, prev, cur)
// print stack contents
// stack is Stack *, _data is a function returning printable data like ( int, char *, char, float ) ...
// format is "%s" "%d" ... according to what data returns
#define Stack_print(stack, _data, format) DLinked_print(stack, last, prev, _data, format)
// stack is Stack *
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define Stack_search(stack, cmp_func, to_find, found_node) DLinked_search(stack, last, prev, cmp_func, to_find, found_node)

#endif /* DA30B4E4_4771_452B_ABA5_8E814ED70200 */