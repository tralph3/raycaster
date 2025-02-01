#ifndef ARRAY_QUEUE
#define ARRAY_QUEUE

#include <stdlib.h>


typedef struct ArrayQueue {
    int *items;
    int max_items;
    int pointer;
} ArrayQueue;

ArrayQueue *array_queue_create(int max_size) {
    ArrayQueue *queue = malloc(sizeof(ArrayQueue));
    queue->items = malloc(sizeof(int) * max_size);
    queue->max_items = max_size;
    queue->pointer = 0;

    return queue;
}

void array_queue_push(ArrayQueue *queue, int element) {
    if (queue->pointer >= queue->max_items)
        return;

    queue->items[queue->pointer] = element;
    ++queue->pointer;
}

int array_queue_pop(ArrayQueue* queue) {
    if (queue->pointer <= 0)
        return 0;

    return queue->items[--queue->pointer];
}

#endif
