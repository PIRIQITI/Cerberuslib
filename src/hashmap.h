/*
 * This source file is part of the liblcthw library. Part of this code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#ifndef E8E25189_ED08_4AAC_A77A_C4DA8A7853DB
#define E8E25189_ED08_4AAC_A77A_C4DA8A7853DB

#define DEFAULT_NUMBER_OF_BUCKETS 100

#define ALLOW_DUPLICATES 1
#define MAKE_SORTED 2

#include <stdint.h>
#include "DArray.h"

typedef int (*Hashmap_compare) (const void *data1, const void *data2);
typedef uint32_t (*Hashmap_hash) (void *data);

typedef struct Hashmap {
    DArray *buckets;
    Hashmap_compare cmp;
    Hashmap_hash hash;
} Hashmap;

typedef struct HashmapNode {
    void *key;
    void *data;
    uint32_t hash;
} HashmapNode;

typedef int (*Hashmap_traverse_cb) (HashmapNode *node);

// create a hashmap
// specify cmp if you want to binary search later ( default is strcmp )
// specify hash_func ( default is fnv1 which hashes C strings )
// number of hashmap buckets as the last argument
Hashmap *Hashmap_create(Hashmap_compare cmp, Hashmap_hash hash_func, int number_of_buckets);
// free HashmapNodes buckets and map ( DOES NOT FREE DATA WHICH YOU INSERTED )
void Hashmap_destroy(Hashmap *map);
// set ( insert ) data in a specific bucket
// flags can be ALLOW_DUPLICATES and MAKE_SORTED either, both, or none ( 0 )
int Hashmap_set(Hashmap *map, void *key, void *data, int flags);
// get a data with key
void *Hashmap_get(Hashmap *map, void *key);
// join 2 hashmaps, handler_func is needed to free data, if you choose not to set ALLOW_DUPLICATES
// pass the references of hashmaps 1 and 2
int Hashmap_join(Hashmap **map1, Hashmap **map2, free_func handler_func, int flags);
// traverse through hashmap and apply some functions to it
int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb);
// delete a data from hashmap and return it
void *Hashmap_delete(Hashmap *map, void *key);

// binary search sorted hashmap
void *Hashmap_binary_search(Hashmap *map, void *key);

// free complex, user created data structures: structs, unions ...
// handler_func is a function you provide for handling (freeing) your own data structures
// pass a reference of hashmap
int Hashmap_free_complex_data(Hashmap **map, free_func handler_func);

// print hashmap contents
// map is Hashmap *, _data is a function returning printable data like ( int, char *, char, float ) ...
// format is "%s" "%d" ... according to what data returns
#define Hashmap_print(map, _data, _format) {\
            if (!map) {\
                log_err("Somehow got map that is NULL.");\
            } else {\
                int __i__ = 0;\
                for (__i__ = 0; __i__ < map->buckets->length; __i__++) {\
                    DArray *array = (DArray *) map->buckets->contents[__i__];\
                    if (!array) {\
                        printf("NULL\n");\
                    } else if (!array->length) {\
                        printf("[ ]\n");\
                    } else {\
                        int __j__ = 0;\
                        printf("[ ");\
                        for (__j__ = 0; __j__ < array->length - 1; __j__++) {\
                            printf(_format ", ", _data(((HashmapNode *) array->contents[__j__])->data));\
                        }\
                        printf(_format " ]\n", _data(((HashmapNode *) array->contents[__j__])->data));\
                    }\
                }\
            }\
        }

#endif /* E8E25189_ED08_4AAC_A77A_C4DA8A7853DB */