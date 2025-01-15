#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <assert.h>
#include <stdlib.h>

typedef struct {
    unsigned int capacity;
    unsigned int count;
    void *items;
} DynArray;

#define DA_INIT_CAP 256

#define da_last(da) (assert((da)->count > 0), (da)->items[(da)->count - 1])

#define da_move(dst, src)                       \
    do {                                        \
        free((dst)->items);                     \
        (dst)->items = (src).items;             \
        (dst)->count = (src).count;             \
        (dst)->capacity = (src).capacity;       \
    } while (0)

#define da_append(da, item)                                             \
    do {                                                                \
        if ((da)->count >= (da)->capacity) {                            \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2; \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Out of memory");             \
        }                                                               \
                                                                        \
        (da)->items[(da)->count++] = (item);                            \
    } while (0)

#define da_append_many(da, new_items, new_items_count)                  \
    do {                                                                \
        if ((da)->count + new_items_count > (da)->capacity) {           \
            if ((da)->capacity == 0) {                                  \
                (da)->capacity = DA_INIT_CAP;                           \
            }                                                           \
            while ((da)->count + new_items_count > (da)->capacity) {    \
                (da)->capacity *= 2;                                    \
            }                                                           \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Out of memory");             \
        }                                                               \
        memcpy((da)->items + (da)->count, new_items, new_items_count*sizeof(*(da)->items)); \
        (da)->count += new_items_count;                                 \
    } while (0)

#endif
