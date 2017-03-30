#include "includes/pool.h"

pool *new_pool(size_t element_size, int element_count) {
    element_size = element_size + sizeof(pool_element);
    void *pool_memory = malloc(element_size * element_count);
    
    pool_element *last_element = pool_memory;
    for(int i = 0; i < element_count; i++) {
        pool_element *element = pool_memory + i * element_size;
        last_element->next = element;
        last_element = element;
    }
    last_element->next = NULL;
    
    pool *p = malloc(sizeof(pool));
    p->pool_memory = pool_memory; 
    p->element_size = element_size;
    p->element_count = element_count;
    p->head = pool_memory;
    p->tail = last_element;
    
    return p;
}

bool pool_full(pool *p) {
    return p->head == NULL;
}

int pool_size(pool *p) {
    return p->element_count;
}

int pool_count(pool *p) {
    if(pool_full(p)) return p->element_count; 

    int free_count = 0;
    pool_element *ptr = p->head;
    while(ptr != p->tail){
        free_count++;
        ptr = ptr->next;
    }
    free_count++;

    return p->element_count - free_count;
}

// extends the size of the pool
void pool_extend(pool *p, int new_count) {
    assert(new_count > p->element_count);
    int old_count = p->element_count;
    p->element_count = new_count;

    bool is_full = pool_full(p);

    // get the head/tail offset
    void *old_memory = p->pool_memory;
    int head_offset = (char *)p->head - (char *)p->pool_memory;
    int tail_offset = (char *)p->tail - (char *)p->pool_memory;
    
    // extend the pool memory
    p->pool_memory = realloc(p->pool_memory, p->element_size * p->element_count);
    assert(p->pool_memory != NULL);
    
    // restore the head/tail
    p->head = (pool_element *)((char *)p->pool_memory + head_offset);
    p->tail = (pool_element *)((char *)p->pool_memory + tail_offset);
    
    // restore the pointers in the free list
    if(!is_full) {
        pool_element *ptr = p->head;
        while(ptr != p->tail) {
            ptr->next = p->pool_memory + ((void *)ptr->next - old_memory);
            ptr = ptr->next;
        }
    }

    // get the first new element
    pool_element *first_new = p->pool_memory + (p->element_size * old_count);
    
    // set up a free list for the new elements
    pool_element *last_element = first_new;
    for(int i = old_count; i < p->element_count; i++) {
        pool_element *element = p->pool_memory + i * p->element_size;
        last_element->next = element;
        last_element = element;
    }
    
    if(is_full) {
        // set the head to the new free list
        p->head = first_new;
    } else {
        // join the old free list and the new free list
        p->tail->next = first_new;
    }

    p->tail = last_element;
}

void *pool_get(pool *p) {
    if (pool_full(p)) pool_extend(p, p->element_count * 2);
    pool_element *element = p->head;
    p->head = p->head->next;
    return element + 1;
}

void pool_release(pool *p, void *element) {
    assert(element > p->pool_memory);
    assert(element < p->pool_memory + p->element_count * p->element_size);
    if (pool_full(p)) {
        p->head = element;
        p->head->next = NULL;
        p->tail = p->head;
    } else {
        p->tail->next = element;
        p->tail = element;
    }
}

void pool_destroy(pool *p) {
    free(p->pool_memory);
}
