#include "includes/pool.h"

// new_pool creates a new pool
pool *new_pool(size_t element_size, int element_count) {
    // allocate space for elements + free list
    element_size = element_size + sizeof(pool_element);
    void *pool_memory = malloc(element_size * element_count);
    
    // set up the free list
    pool_element *last_element = pool_memory;
    for(int i = 0; i < element_count; i++) {
        pool_element *element = pool_memory + i * element_size;
        last_element->next = element;
        last_element = element;
    }
    last_element->next = NULL;
    
    // construct the pool data
    pool *p = malloc(sizeof(pool));
    p->pool_memory = pool_memory; 
    p->element_size = element_size;
    p->element_count = element_count;
    p->head = pool_memory;
    p->tail = last_element;
    
    return p;
}

// pool_full returns true if the pool is full
bool pool_full(pool *p) {
    return p->head == NULL;
}

// pool_size returns the size (amount of elements that can be allocated) of the pool
int pool_size(pool *p) {
    return p->element_count;
}

// pool_count returns the amount of elements in the pool
int pool_count(pool *p) {
    // if the pool is full no need to look at free list
    if(pool_full(p)) return p->element_count; 

    // count the amount of elements in the free list
    int free_count = 0;
    pool_element *ptr = p->head;
    while(ptr != p->tail){
        free_count++;
        ptr = ptr->next;
    }
    free_count++;

    return p->element_count - free_count;
}

// pool_extend extends the size of the pool
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

// pool_get gets a new element from the pool, increasing the pools size if its full.
void *pool_get(pool *p) {
    if (pool_full(p)) pool_extend(p, p->element_count * 2);
    pool_element *element = p->head;
    p->head = p->head->next;
    return element + 1;
}

// pool_release releases element back into pool to be reused
void pool_release(pool *p, void *element) {
    // Check element is within bounds
    assert(element > p->pool_memory);
    assert(element < p->pool_memory + p->element_count * p->element_size);
    
    // Move pointer back to free list data
    pool_element *list_element = element;
    list_element--;

    // Add to free list
    if (pool_full(p)) {
        // Free list is empty so start a new free list
        p->head = list_element;
        p->head->next = NULL;
        p->tail = p->head;
    } else {
        // Append to free list
        p->tail->next = list_element;
        p->tail = list_element;
    }
}

// pool_destroy frees the pools memory
void pool_destroy(pool *p) {
    free(p->pool_memory);
}
