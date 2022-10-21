/*
 * This source file is part of the liblcthw library. Part of this code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#include <string.h>
#include "hashmap.h"
#include "dbg.h"

static int default_compare(const void *a, const void *b)
{
    return strcmp(a, b);
}

const uint32_t FNV_PRIME = 16777619;
const uint32_t FNV_OFFSET_BASIS = 2166136261;

// fnv1a hash
static uint32_t default_hash(void *data)
{
    // bstring obj = (bstring) data;
    uint32_t hash = FNV_OFFSET_BASIS;

    int i = 0, len = strlen((char *) data);
    for (i = 0; i < len; i++) {
        hash ^= i < len ? ((char *) data)[i] : 0;
        hash *= FNV_PRIME;
    }

    return hash;
}

Hashmap *Hashmap_create(Hashmap_compare cmp, Hashmap_hash hash_func, int number_of_buckets)
{
    Hashmap *map = calloc(1, sizeof(Hashmap));
    check_mem(map);

    map->cmp = cmp == NULL ? default_compare : cmp;
    map->hash = hash_func == NULL ? default_hash : hash_func;

    if (number_of_buckets) {
        map->buckets = DArray_create(sizeof(DArray *), number_of_buckets, map->cmp);
    } else {
        map->buckets = DArray_create(sizeof(DArray *), (int) DEFAULT_NUMBER_OF_BUCKETS, map->cmp);
    }

    check_mem(map->buckets);
    map->buckets->length = map->buckets->capacity;

    return map;

error:
    if (map) {
        Hashmap_destroy(map);
    }

    return NULL;
}

void Hashmap_destroy(Hashmap *map)
{
    check(map != NULL, "Somehow got map that is NULL.");

    int i = 0;
    int j = 0;

    if (map->buckets) {
        for (i = 0; i < map->buckets->capacity; i++) {
            DArray *bucket = DArray_get(map->buckets, i);
            if (bucket) {
                for (j = 0; j < bucket->length; j++) {
                    free(DArray_get(bucket, i));
                }
                DArray_destroy(&bucket);
            }
        }
        DArray_destroy(&map->buckets);
    }
    free(map);

error:
    return;
}

static inline HashmapNode *Hashmap_node_create(int hash, void *key, void *data)
{
    HashmapNode *node = calloc(1, sizeof(HashmapNode));
    check_mem(node);

    node->key = key;
    node->data = data;
    node->hash = hash;

    return node;

error:
    return NULL;
}

static inline DArray *Hashmap_find_bucket(Hashmap *map, void *key, int create, uint32_t *hash_out)
{
    uint32_t hash = map->hash(key);
    int bucket_n = hash % map->buckets->capacity;
    check(bucket_n >= 0, "Invalid bucket found: %d", bucket_n);
    // store it to take it out of the function for further use
    *hash_out = hash;

    DArray *bucket = DArray_get(map->buckets, bucket_n);

    if (!bucket && create) {
        // new bucket is getting created
        bucket = DArray_create(sizeof(void *), map->buckets->capacity, map->cmp);
        check_mem(bucket);
        DArray_set(map->buckets, bucket_n, bucket);
    }

    return bucket;

error:
    return NULL;
}

int Hashmap_set(Hashmap *map, void *key, void *data, int flags)
{
    check(map != NULL, "Somehow got map that is NULL.");
    check(key != NULL, "Somehow got key that is NULL.");
    check(data != NULL, "Somehow got data that is NULL.");
    check(flags < 4, "Invalid flags");

    if (!(flags & ALLOW_DUPLICATES) && Hashmap_get(map, key)) return -1; // flags = 1 ? allow : not allow

    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 1, &hash);
    check(bucket, "Couldn't create bucket.");

    HashmapNode *node = Hashmap_node_create(hash, key, data);
    check_mem(node);

    switch (flags) {
        case MAKE_SORTED: // 2
            DArray_sorted_insert(bucket, node);
            break;
        case ALLOW_DUPLICATES | MAKE_SORTED: // 3
            DArray_sorted_insert(bucket, node);
            break;
        default: // 0 and 1
            DArray_push(bucket, node);
            break;
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

static inline HashmapNode *Hashmap_get_node(Hashmap *map, uint32_t hash, DArray *bucket, void *key)
{
    int i = 0;

    for (i = 0; i < bucket->length; i++) {
        debug("TRY: %d", i);
        HashmapNode *node = DArray_get(bucket, i);
        if (node->hash == hash && map->cmp(node->key, key) == 0) {
            return node;
        }
    }

    return NULL;
}

void *Hashmap_get(Hashmap *map, void *key)
{
    check(map != NULL, "Somehow got map that is NULL.");
    check(key != NULL, "Somehow got key that is NULL.");
    
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (!bucket) return NULL;

    HashmapNode *node = Hashmap_get_node(map, hash, bucket, key);
    check(node != NULL, "Somehow got node that is NULL, when it shouldn't be.");

    return node->data;

error:
    return NULL;
}

int Hashmap_join(Hashmap **map1, Hashmap **map2, free_func handler_func, int flags)
{
    check(map1 != NULL, "Somehow got address of the map1 that is NULL.");
    check(map2 != NULL, "Somehow got address of the map2 that is NULL.");
    check(*map1 != NULL, "Somehow got map1 that is NULL.");
    check(*map2 != NULL, "Somehow got map2 that is NULL.");
    check(flags < 4, "Invalid flags");

    DArray *array = NULL;
    HashmapNode *node = NULL;

    int i = 0, j = 0, rc = 0;
    for (i = 0; i < (*map2)->buckets->length; i++) {
        array = (DArray *) (*map2)->buckets->contents[i];
        if (array) {
            for (j = 0; j < array->length; j++) {
                node = (HashmapNode *) array->contents[j];
                rc = Hashmap_set(*map1, node->key, node->data, flags);
                if (rc == CERB_ERR) {
                    debug("freeing duplicate: %s", (char *) node->data);
                    handler_func(node->key);
                }
                free(node);
            }
            free(array->contents);
            free(array);
        }
    }
    free((*map2)->buckets->contents);
    free((*map2)->buckets);
    free(*map2);
    
    map2 = NULL;

    return CERB_OK;
        
error:
    return CERB_ERR;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb)
{
    check(map != NULL, "Somehow got map that is NULL.");
    check(traverse_cb != NULL, "Somehow got traverse_cb (callback) that is NULL.");

    int i = 0;
    int j = 0;
    int rc = 0;

    for (i = 0; i < map->buckets->capacity; i++) {
        DArray *bucket = DArray_get(map->buckets, i);
        if (bucket) {
            for (j = 0; j < bucket->length; j++) {
                HashmapNode *node = DArray_get(bucket, j);
                rc = traverse_cb(node);
                if (rc == CERB_OK) {
                    return rc;
                }
            }
        }
    }

    return CERB_OK;

error:
    return CERB_ERR;
}

void *Hashmap_delete(Hashmap *map, void *key)
{
    check(map != NULL, "Somehow got map that is NULL.");
    check(key != NULL, "Somehow got key that is NULL.");
    
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (!bucket) return NULL;

    HashmapNode *node = Hashmap_get_node(map, hash, bucket, key);
    if (!node) return NULL;

    void *data = node->data;

    // we pop the last element
    HashmapNode *ending = DArray_pop(bucket);

    // and if it is not deleted one we just replace deleted element with this
    if (ending != node) {
        int i = DArray_get_pos(bucket, node);
        DArray_set(bucket, i, ending);
    }

    free(node);

    return data;

error:
    return NULL;
}

void *Hashmap_binary_search(Hashmap *map, void *key)
{
    check(map != NULL, "Somehow got map that is NULL.");
    check(key != NULL, "Somehow got key that is NULL.");

    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (!bucket) return NULL;

    int high = bucket->length - 1, low = 0, middle;
    while (low <= high) {
        middle = (high + low) / 2;
        if (map->cmp(((HashmapNode *) bucket->contents[middle])->data, key) < 0) {
            low = middle + 1;
        } else if (map->cmp(((HashmapNode *) bucket->contents[middle])->data, key) > 0) {
            high = middle - 1;
        } else {
            return ((HashmapNode *) bucket->contents[middle])->data;
        }
    }

error: // fall through
    return NULL;
}

int Hashmap_free_complex_data(Hashmap **map, free_func handler_func)
{
    check(map != NULL, "Somehow got address of map that is NULL.");
    check(*map != NULL, "Somehow got map that is NULL.");
    check(handler_func != NULL, "Somehow got handler_func (callback) that is NULL.");

    DArray *array = NULL;
    HashmapNode *node = NULL;

    int i = 0, j = 0;
    for (i = 0; i < (*map)->buckets->length; i++) {
        array = (DArray *) (*map)->buckets->contents[i];
        if (array) {
            for (j = 0; j < array->length; j++) {
                node = (HashmapNode *) array->contents[j];
                // pointers node->key and node->data point to same memory and hence this line is written so
                handler_func(node->key); // WATCH THIS WITH BIG EYES
                // debug("pointer to node->key = %p", node->key);
                free(node);
                // debug("bucket->length = %d", array->length);
            }
            free(array->contents);
            free(array);
        }
    }
    free((*map)->buckets->contents);
    free((*map)->buckets);
    free(*map);

    *map = NULL;

    return CERB_OK;

error:
    return CERB_ERR;
}