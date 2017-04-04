#include "includes/queue.h"

queue *new_queue(size_t element_size) {
    queue *q = malloc(sizeof(queue));
    element_size += sizeof(queue_item);
    q->element_size = element_size;
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
    return q;
}

int queue_size(queue *q) {
    return q->size;
}

void *queue_push_front(queue *q) {
    q->size++;
    queue_item *q_item = malloc(q->element_size);
    
    q_item->prev = NULL;
    if (q->first == NULL) {
        q_item->next = NULL;
        q->last = q_item;
    } else {
        q_item->next = q->first;
        q->first->prev = q_item;
    }
    q->first = q_item;

    return q_item + 1; // move past item header
}

void *queue_push_back(queue *q) {
    q->size++;
    queue_item *q_item = malloc(q->element_size);
    
    q_item->next = NULL;
    if(q->first == NULL) {
        q_item->prev = NULL;
        q->first = q_item;
    } else {
        q_item->prev = q->last;
        q->last->next = q_item;
    }
    q->last = q_item;
    
    return q_item + 1; // move past item header
}

void *queue_pop_front(queue *q) {
    if(q->first == NULL) return NULL;
    q->size--;
    queue_item *q_item = q->first;
    if (q->first == q->last) q->last = NULL;
    q->first->prev = NULL;
    q->first = q->first->next;
    return q_item + 1; // move past item header
}

void *queue_pop_back(queue *q) {
    if(q->last == NULL) return NULL;
    q->size--;
    queue_item *q_item = q->last;
    if (q->last == q->first) q->first = NULL;
    q->last->next = NULL;
    q->last = q->last->prev;
    return q_item + 1; // move past item header
}

void queue_free_item(void *item) {
    queue_item *q_item = item;
    free(q_item - 1);
}

void queue_destroy(queue *q) {
    while(q->first != NULL) {
        queue_item *next = q->first->next;
        free(q->first);
        q->first = next;
    }
}
