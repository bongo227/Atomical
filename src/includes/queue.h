#pragma once

#include "all.h"

typedef struct {
    void *queue_memory;
    size_t element_size;
    int queue_capacity;
    int queue_size;
    int head_ptr;
} queue;

queue *new_queue(size_t element_size, int queue_capacity);
bool queue_full(queue *q);
void queue_extend(queue *q);
void *queue_enqueue(queue *q);
void *queue_dequeue(queue *q);
void queue_destroy(queue *q);