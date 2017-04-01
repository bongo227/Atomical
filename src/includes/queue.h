#pragma once

#include "all.h"

struct queue_item {
    struct queue_item *prev;
    struct queue_item *next;
};

typedef struct queue_item queue_item;

typedef struct {
    void *memory;
    size_t element_size;
    int size;
    queue_item *first;
    queue_item *last;
} queue;

queue *new_queue(size_t element_size);
int queue_size(queue *q);
void *queue_push_front(queue *q);
void *queue_push_back(queue *q);
void *queue_pop_front(queue *q);
void *queue_pop_back(queue *q);
void queue_free_item(void *item);
void queue_destroy(queue *q);