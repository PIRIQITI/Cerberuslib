#ifndef D9763356_738A_4CC3_81EE_E99D6E5B226A
#define D9763356_738A_4CC3_81EE_E99D6E5B226A

#include "doubly_linked.h"

#define Queue DLinked

// get how many elements queue has
#define Queue_get_count(queue) DLinked_get_count(queue)

// create a queue 
#define Queue_create() DLinked_create(NULL)
// unshift void *data in queue
#define Queue_unshift(queue, data) DLinked_unshift(queue, data)
// pop and return from queue
#define Queue_pop(queue) DLinked_pop(queue)
// if data structure, which nodes contain is complex (for example struct containing pointers)
// use this function which iterates through list and applies your handler_func to all of it's *data fields
#define Queue_free_complex_data(queue, handler_func) DLinked_free_complex_data(queue, handler_func)

// iterate through queue
// queue is Queue *, cur is current node's name you specify
#define Queue_iter(queue, cur) DLinked_iter(queue, first, next, cur)
// print queue contents
// queue is Queue *, _data is a function returning printable data like ( int, char *, char, float ) ...
// format is "%s" "%d" ... according to what data returns
#define Queue_print(queue, _data, format) DLinked_print(queue, first, next, _data, format)
// queue is Queue *
// cmp_fucn is a function pointer which compares search data and every node's data
// to find is the data you are looking for and found node is the variable name you want found data to be in
#define Queue_search(queue, cmp_func, to_find, found_node) DLinked_search(queue, first, next, cmp_func, to_find, found_node)

#endif /* D9763356_738A_4CC3_81EE_E99D6E5B226A */