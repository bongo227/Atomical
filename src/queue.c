#include "includes/queue.h"

queue *new_queue(size_t element_size, int queue_capacity) {
    assert(queue_capacity > 0);
    queue *q = malloc(sizeof(queue));
    q->queue_memory = malloc(element_size * queue_capacity);
    q->element_size = element_size;
    q->queue_capacity = queue_capacity;
    q->queue_size = 0;
    q->head_ptr = 0;

    return q;
}

bool queue_full(queue *q) {
    return q->queue_capacity == (q->queue_size + q->head_ptr);
}

void queue_extend(queue *q) {
    q->queue_capacity *= 2;
    q->queue_memory = realloc(q->queue_memory, q->element_size * q->queue_capacity);
}

void *queue_enqueue(queue *q) {
    if (queue_full(q)) queue_extend(q);
    void *element = q->queue_memory + q->element_size * q->queue_size;
    q->queue_size++;
    return element;
}

void *queue_dequeue(queue *q) {
    void *element = q->queue_memory + q->element_size * q->head_ptr;
    q->head_ptr++;
    return element;
}

void queue_destroy(queue *q) {
    free(q->queue_memory);
}
