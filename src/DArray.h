/*
 * This source file is part of the liblcthw library. Part of this code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#ifndef FE64FB78_C357_4BF2_988D_468BA50B4D80
#define FE64FB78_C357_4BF2_988D_468BA50B4D80

#define CERB_ERR -1
#define CERB_OK 0

#include <stdint.h>
#include <stdlib.h>
#include "dbg.h"

typedef void (*free_func) (void *data);
typedef int (*cmp_template) (const void *data1, const void *data2);

typedef struct DArray {
    int capacity;
    int length;
    size_t element_size;
    uint16_t expand_rate;
    cmp_template cmp_func;
    void **contents;
} DArray;

// create a DArray. element size is 8, initial capacity could be any user specified number
// cmp func is a function pointer and might be used to make sorted insertion or sort later and apply binary search
DArray *DArray_create(size_t element_size, int initial_capacity, cmp_template cmp_func);
// expands DArray's capacity by DEFAULT_EXPAND_RATE
int DArray_expand(DArray *array);
// contracts DArray's capacity by DEFAULT EXPAND RATE
int DArray_contract(DArray *array);
// push data at the end of an array
int DArray_push(DArray *array, void *data);
// pop data from the end of an array
void *DArray_pop(DArray *array);
// shrink array to fit exactly the amount of elements it has
int DArray_shrink_to_fit(DArray *array);
// split array in two from position to position
DArray *DArray_split(DArray **array, int from_position, int to_position);
// join 2 arrays
int DArray_join(DArray **array1, DArray **array2);
// change DEFAULT EXPAND RATE
int DArray_set_expand_rate(DArray *array, uint16_t new_rate);

// free array->contents and array itsself ( THIS DOES NOT FREE THE DATA IN THE contents )
int DArray_destroy(DArray **array);
// free the data in contents ( THIS FUNCTION SHOULD ONLY BE USED ON BASIC DATA STRUCTURES: int, double, char * ... )
int DArray_clear(DArray **array);
// DArray_destroy and DArray_clear combined
int DArray_clear_destroy(DArray **array);

// sort an array
int DArray_sort(DArray *array);
// sorted insert in array ( SHOULD BE INSERTED WITH THIS FUNCTION ONLY, IF YOU WANT TO HAVE SORTED ARRAY OR AFTER DArray_sort )
int DArray_sorted_insert(DArray *array, void *data);
// apply fast binary search ( SHOULD BE SORTED IF YOU USE )
int DArray_binary_search(DArray *array, void *search_data);

// free complex, user created data structures: structs, unions ...
// handler_func is a function you provide for handling (freeing) your own data structures
int DArray_free_complex_data(DArray **array, free_func handler_func);


int DArray_free_array(DArray **array);

// set contents[i] to data
static inline int DArray_set(DArray *array, int i, void *data)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(i < array->length, "Couldn't set element number that hasn't been added to list previously.");
    check(i >= 0, "Enter valid element position.");

    array->contents[i] = data;

    return CERB_OK;

error:
    return CERB_ERR;
}

// get position ( i ) of search data
static inline int DArray_get_pos(DArray *array, void *search_data)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(search_data != NULL, "Somehow got search_data that is NULL.");

    int i = 0;
    for (i = 0; i < array->length; i++) {
        if (array->contents[i] == search_data) {
            return i;
        }
    }

error:
    return CERB_ERR;
}

// get data at position i
static inline void *DArray_get(DArray *array, int i)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(i < array->length, "Couldn't get element number that hasn't been added to list previously.");
    check(i >= 0, "Enter valid element position.");

    return array->contents[i];

error:
    return NULL;
}

// set contents[i] = NULL and return data which was in there
static inline void *DArray_remove(DArray *array, int i)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(i < array->length, "Couldn't get element number that hasn't been added to list previously.");
    check(i >= 0, "Enter valid element position.");

    void *data = array->contents[i];
    array->contents[i] = NULL;

    return data;

error:
    return NULL;
}

// _data returns some printable data like (int, char, char * (for %s) etc...)
// _format is "%s", "%d" ...
#define DArray_print(array, _data, _format) {\
            if (!(array)) {\
                log_err("Somehow got array that is NULL.");\
            } else if (!(array)->length) {\
                printf("[ ]\n");\
            } else {\
                int i = 0;\
                printf("\n[ ");\
                for (i = 0; i < (array)->length - 1; i++) { printf(_format ", ", _data((array)->contents[i])); }\
                printf(_format " ]\n", _data((array)->contents[i]));\
            }\
        }

// #define DArray_search(array, data)

#define DArray_last(A) ((A)->contents[(A)->length - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_length(A) ((A)->length)
#define DArray_capacity(A) ((A)->capacity)

#define DEFAULT_EXPAND_RATE 300

#endif /* FE64FB78_C357_4BF2_988D_468BA50B4D80 */