/*
 * This source file is part of the liblcthw library. Part of this code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#include <string.h>
#include "DArray.h"

static inline int default_cmp(const void *data1, const void *data2)
{
    return strcmp(data1, data2);
}

DArray *DArray_create(size_t element_size, int initial_capacity, cmp_template cmp_func)
{
    DArray *array = NULL;
    check(element_size != 0, "Couldn't make 0 element sized array.");
    check(initial_capacity >= 0, "Initial capacity Can't be negative");

    array = malloc(1 * sizeof(DArray));
    check_mem(array);

    array->element_size = element_size;
    array->capacity = initial_capacity;
    array->expand_rate = (uint16_t) DEFAULT_EXPAND_RATE;
    array->length = 0;
    array->cmp_func = cmp_func == NULL ? default_cmp : cmp_func;

    array->contents = calloc(initial_capacity, sizeof(void *));
    check_mem(array->contents);

    return array;

error:
    if (array) {
        free(array);
    }
    return NULL;
}

/* pushing and popping */

int DArray_push(DArray *array, void *data)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");

    if (array->length == array->capacity) {
        int rc = DArray_expand(array);
        check(rc, "Couldn't expand array, you won't be able insert past current length.");
    }

    array->contents[array->length] = data;
    array->length++;

    return CERB_OK;

error:
    return CERB_ERR;
}

void *DArray_pop(DArray *array)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(array->length > 0, "Couldn't pop from empty array.");

    void *data = array->contents[array->length - 1];
    array->length--;

    // when contracted after expansion the minimum capacity stays DEFAULT_EXPAND_RATE
    if (array->length <= array->capacity - array->expand_rate) {
        int rc = DArray_contract(array);
        check(rc, "Couldn't contract array to new size, but you'll still be able to pop.");
    }

    return data;

error:
    return NULL;
}

/* resize operatins */

static inline int DArray_resize(DArray *array, size_t new_capacity)
{
    check(new_capacity <= INT32_MAX, "Couldn't expand past max capacity %d AKA INT32_MAX", INT32_MAX);

    void *contents = realloc(array->contents, new_capacity * sizeof(void *));
    check_mem(contents);

    array->contents = contents;
    array->capacity = new_capacity;

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_expand(DArray *array)
{
    check(array != NULL, "Somehow got array that is NULL.");

    size_t new_capacity = array->length + array->expand_rate;
    
    int rc = DArray_resize(array, new_capacity);
    if (!rc) goto error;

    memset(array->contents + array->length, 0, array->capacity);

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_contract(DArray *array)
{
    check(array != NULL, "Somehow got array that is NULL.");
    
    int rc = DArray_resize(array, (size_t) (array->capacity - array->expand_rate));
    if (!rc) goto error;

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_shrink_to_fit(DArray *array)
{
    check(array != NULL, "Somehow got array that is NULL.");

    int rc = DArray_resize(array, (size_t) array->length);
    check(rc, "Couldn't shrink the array.");

    return CERB_OK;

error:
    return CERB_ERR;
}

/* splitting and joining */

int DArray_join(DArray **array1, DArray **array2)
{
    check(array1 != NULL, "Somehow got address of array1 that is NULL.");
    check(*array1 != NULL, "Somehow got array1 that is NULL.");
    check(array2 != NULL, "Somehow got address of array2 that is NULL.");
    check(*array2 != NULL, "Somehow got array2 that is NULL.");

    // if array1 capacity is insufficient to hold both array elements
    // then we expand and capacity is the exact amount needed to hold both array elements
    if ((*array1)->capacity < (*array1)->length + (*array2)->length) {
        int rc = DArray_resize(*array1, (size_t) ((*array1)->length + (*array2)->length));
        check(rc, "Couldn't expand the array to fit new elements. arrays haven't been changed.");
    }

    memcpy((*array1)->contents + (*array1)->length, (*array2)->contents, sizeof(void *) * (*array2)->length);
    
    (*array1)->length = (*array1)->capacity;

    free((*array2)->contents);
    free(*array2);
    *array2 = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

DArray *DArray_split(DArray **array, int from_position, int to_position)
{
    check(array != NULL, "Somehow got address of array1 that is NULL.");
    check(*array != NULL, "Somehow got array1 that is NULL.");
    check(from_position >= 0 && from_position < (*array)->length - 1, "Invalid from_position.");
    check(to_position <= (*array)->length && to_position >= 0, "Invalid to_position.");
    check(from_position < to_position, "Invalid positions.");

    int new_array_length = to_position - from_position;

    DArray *new_array = DArray_create(8, new_array_length, (*array)->cmp_func);
    check(new_array != NULL, "Couldn't split the array.");
    new_array->length = new_array->capacity;

    memcpy(new_array->contents, (*array)->contents + from_position, sizeof(void *) * new_array->length);

    int i = 0, j = 0;
    for (i = from_position; j < (*array)->length - to_position; i++, j++) {
        (*array)->contents[i] = (*array)->contents[j + to_position];
    }

    int rc = DArray_resize(*array, (*array)->length - new_array_length);
    if (!rc) {
        log_err("Failed resize the array, thus it hasn't been splitted.");
        free(new_array->contents);
        free(new_array);
        goto error;
    }
    (*array)->length = (*array)->capacity;
    
    return new_array;
    
error:
    return NULL;
}

int DArray_set_expand_rate(DArray *array, uint16_t new_rate)
{
    check(array != NULL, "Somehow got array that is NULL.");

    array->expand_rate = new_rate;

    return CERB_OK;

error:
    return CERB_ERR;
}

/* freeing operations */

int DArray_destroy(DArray **array)
{
    check(array != NULL, "Somehow got address of array that is NULL.");
    check(array != NULL, "Somehow got array that is NULL.");

    free((*array)->contents);
    free(*array);
    *array = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_clear(DArray **array)
{
    check(array != NULL, "Somehow got address of array that is NULL.");
    check(*array != NULL, "Somehow got array that is NULL.");

    int i = 0;
    for (i = 0; i < (*array)->length; i++) {
        free((*array)->contents[i]);
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

// if this seems slow you can unite codes from two functions to bypass double check of address and pointer
int DArray_clear_destroy(DArray **array)
{   
    int rc = DArray_clear(array);
    check(rc, "Couldn't clear the array.");

    // if clear was successful theres no need to check for rc here
    DArray_destroy(array);

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_free_complex_data(DArray **array, free_func handler_func)
{
    check(array != NULL, "Somehow got address of array that is NULL.");
    check(*array != NULL, "Somehow got array that is NULL.");

    int i = 0;
    for (i = 0; i < (*array)->length; i++) {
        handler_func((*array)->contents[i]);
    }
    free((*array)->contents);
    free(*array);

    *array = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_free_array(DArray **array)
{
    check(array != NULL, "Somehow got address of array that is NULL.");
    check(*array != NULL, "Somehow got array that is NULL.");

    free((*array)->contents);

    free(*array);

    *array = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}

/* sorting and binary searching */

int DArray_sort(DArray *array)
{
    check(array != NULL, "Somehow got array that is NULL.");

    int i = 0, j = 0;
    for (i = 0; i < array->length; i++) {
        for (j = 0; j < array->length; j++) { // see this
            if (array->cmp_func(array->contents[i], array->contents[j]) < 0) {
                void *temp = array->contents[i];
                array->contents[i] = array->contents[j];
                array->contents[j] = temp;
            }
        }
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

int DArray_sorted_insert(DArray *array, void *data)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");

    if (array->length == array->capacity) DArray_expand(array);

    int i = 0, j = array->length;
    for (i = 0; i < array->length; i++) {
        if (array->cmp_func(array->contents[i], data) >= 0) {
            for (; j > i; j--) {
                array->contents[j] = array->contents[j - 1];
            }
            array->contents[j] = data;
            array->length++;
            return 1;
        }
    }

    // if data is greater than everything that exists in array the we just push it
    array->contents[array->length++] = data;

    return CERB_OK;

error:
    return CERB_ERR;
}

// data has to be sorted for this to work
int DArray_binary_search(DArray *array, void *search_data)
{
    check(array != NULL, "Somehow got array that is NULL.");
    check(search_data != NULL, "Couldn't search for NULL.");

    int low = 0, high = array->length - 1;
    int middle = 0;
    while (low <= high) {
        middle = (low + high) / 2;
        if (array->cmp_func(array->contents[middle], search_data) < 0) {
            low = middle + 1;
        } else if (array->cmp_func(array->contents[middle], search_data) > 0) {
            high = middle - 1;
        } else {
            return middle;
        }
    }
    log_info("Element was not found in list.");

error: // fall through
    return CERB_ERR;
}