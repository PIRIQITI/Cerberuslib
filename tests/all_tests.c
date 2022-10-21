#include "minunit.h"
#include "dbg.h"
#include "singly_linked.h"
#include "doubly_linked.h"
#include "DArray.h"
#include "hashmap.h"
#include "stack.h"
#include "queue.h"
#include <string.h>

// static int length;

void handler_func_ALL(void *data)
{
    free(data);
}

int cmp_func_ALL(void *data1, void *data2)
{
    return strcmp((char *) data1, (char *) data2);
}




// data for test cases

char *get_test_data(const char *cstr)
{
    int length = strlen(cstr);
    char *data = malloc(length + 1);

    check_mem(data);

    for (; length > -1; length--) {
        data[length] = cstr[length];
    }

    return data;

error:
    return NULL;
}


SLinked *S_linked = NULL;
DLinked *D_linked = NULL;
DArray *D_array = NULL;
Hashmap *map = NULL;
Stack *stack = NULL;
Queue *queue = NULL;


char *test1 = NULL;
char *test2 = NULL;
char *test3 = NULL;
char *test4 = NULL;
char *test5 = NULL;

int rc = 0;
void *data = NULL;

// test singly linked list

int create_test_data()
{
    test1 = get_test_data("test 1");
    test2 = get_test_data("test 2");
    test3 = get_test_data("test 3");
    test4 = get_test_data("test 4");
    test5 = get_test_data("test 5");

    if (test1 == NULL || test2 == NULL || test3 == NULL || test4 == NULL || test5 == NULL) return -1;

    return 0;
}

// test singly linked list

char *test_create_SL()
{
    S_linked = SLinked_create(NULL);
    mu_assert(S_linked != NULL, "Failed to create list.");

    return NULL;
}

char *test_push_SL()
{
    rc = SLinked_push(S_linked, test1);
    mu_assert(rc != CERB_ERR, "push failed.");

    rc = SLinked_push(S_linked, test2);
    mu_assert(rc != CERB_ERR, "push failed.");

    mu_assert(S_linked->count == 2, "wrong count on push.");

    return NULL;
}

char *test_pop_SL()
{
    data = SLinked_pop(S_linked);
    mu_assert(data != NULL, "pop failed.");

    mu_assert(S_linked->count == 1, "wrong count on pop.");

    return NULL;
}

char *test_remove_SL()
{
    data = SLinked_remove(S_linked, S_linked->first);
    mu_assert(data != NULL, "remove failed.");

    mu_assert(S_linked->count == 0, "wrong count on remove.");

    return NULL;
}

char *test_free_list_SL()
{
    rc = SLinked_free_list(&S_linked);
    mu_assert(rc != CERB_ERR, "error while freeing.");

    return NULL;
}

// test doubly linked list

char *test_create_DL()
{
    D_linked = DLinked_create(NULL);
    mu_assert(D_linked != NULL, "Failed to create list.");

    return NULL;
}

char *test_push_DL()
{
    rc = DLinked_push(D_linked, test1);
    mu_assert(rc != CERB_ERR, "push failed.");

    rc = DLinked_push(D_linked, test2);
    mu_assert(rc != CERB_ERR, "push failed.");

    mu_assert(D_linked->count == 2, "wrong count on push.");

    return NULL;
}

char *test_pop_DL()
{
    data = DLinked_pop(D_linked);
    mu_assert(data != NULL, "pop failed.");

    mu_assert(D_linked->count == 1, "wrong count on pop.");

    return NULL;
}

char *test_remove_DL()
{
    data = DLinked_remove(D_linked, D_linked->first);
    mu_assert(data != NULL, "remove failed.");

    mu_assert(D_linked->count == 0, "wrong count on remove.");

    return NULL;
}

char *test_free_list_DL()
{
    rc = DLinked_free_list(&D_linked);
    mu_assert(rc != CERB_ERR, "error while freeing.");

    return NULL;
}

// test dynamic array

char *test_create_DA()
{
    D_array = DArray_create(8, 5, NULL);
    mu_assert(D_array != NULL, "failed to create array.");

    return NULL;
}

char *test_push_DA()
{
    rc = DArray_push(D_array, test1);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(D_array->length == 1, "wrong count on push.");

    DArray_push(D_array, test2);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(D_array->length == 2, "wrong count on push.");
    
    DArray_push(D_array, test3);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(D_array->length == 3, "wrong count on push.");
    
    DArray_push(D_array, test4);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(D_array->length == 4, "wrong count on push.");

    DArray_push(D_array, test5);
    mu_assert(rc != CERB_ERR, "push failed.");
    mu_assert(D_array->length == 5, "wrong count on push.");

    return NULL;
}

char *test_pop_DA()
{
    data = DArray_pop(D_array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(D_array->length == 4, "wrong count on pop.");

    data = DArray_pop(D_array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(D_array->length == 3, "wrong count on pop.");

    data = DArray_pop(D_array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(D_array->length == 2, "wrong count on pop.");

    data = DArray_pop(D_array);
    mu_assert(data != NULL, "pop failed.");
    mu_assert(D_array->length == 1, "wrong count on pop.");

    return NULL;
}

char *test_free_array_DA()
{
    rc = DArray_free_array(&D_array);
    mu_assert(rc != CERB_ERR, "error while freeing.");

    return NULL;
}

// test hashmap

char *test_create_HM()
{
    map = Hashmap_create(NULL, NULL, 10);
    mu_assert(map != NULL, "failed to create map.");

    return NULL;
}

char *test_set_HM()
{
    rc = Hashmap_set(map, test1, test1, MAKE_SORTED | ALLOW_DUPLICATES);
    mu_assert(rc != CERB_ERR, "failed to set.");

    rc = Hashmap_set(map, test2, test2, MAKE_SORTED | ALLOW_DUPLICATES);
    mu_assert(rc != CERB_ERR, "failed to set.");

    rc = Hashmap_set(map, test3, test3, MAKE_SORTED | ALLOW_DUPLICATES);
    mu_assert(rc != CERB_ERR, "failed to set.");

    rc = Hashmap_set(map, test4, test4, MAKE_SORTED | ALLOW_DUPLICATES);
    mu_assert(rc != CERB_ERR, "failed to set.");

    rc = Hashmap_set(map, test5, test5, MAKE_SORTED | ALLOW_DUPLICATES);
    mu_assert(rc != CERB_ERR, "failed to set.");

    return NULL;
}

void handler_func(void *data) // for complex free
{
    free(data);
}

char *test_delete_HM()
{
    // start freeing as next function will be the last

    data = Hashmap_delete(map, test3);
    mu_assert(data != NULL, "failed to delete.");
    free(data);

    data = Hashmap_delete(map, test5);
    mu_assert(data != NULL, "failed to delete.");
    free(data);

    data = Hashmap_delete(map, test1);
    mu_assert(data != NULL, "failed to delete.");
    free(data);

    return NULL;
}

char *test_free_complex_data_HM()
{
    rc = Hashmap_free_complex_data(&map, handler_func);
    mu_assert(rc != CERB_ERR, "failed to free.");

    return NULL;
}

char *all_tests()
{
    rc = create_test_data();
    if (rc == -1) return "Failed to create test data.";

    mu_suite_start();

    mu_run_test(test_create_SL);
    mu_run_test(test_push_SL);
    mu_run_test(test_pop_SL);
    mu_run_test(test_remove_SL);
    mu_run_test(test_free_list_SL);

    mu_run_test(test_create_DL);
    mu_run_test(test_push_DL);
    mu_run_test(test_pop_DL);
    mu_run_test(test_remove_DL);
    mu_run_test(test_free_list_DL);

    mu_run_test(test_create_DA);
    mu_run_test(test_push_DA);
    mu_run_test(test_pop_DA);
    mu_run_test(test_free_array_DA);

    mu_run_test(test_create_HM);
    mu_run_test(test_set_HM);
    mu_run_test(test_delete_HM);
    mu_run_test(test_free_complex_data_HM);

    return NULL;
}

RUN_TESTS(all_tests);