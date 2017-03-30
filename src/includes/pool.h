struct pool_element;
typedef struct pool_element pool_element;

struct pool_element {
    pool_element *next;
};

typedef struct {
    void *pool_memory;
    size_t element_size;
    int element_count;

    pool_element *head;
    pool_element *tail;
} pool;

pool *new_pool(size_t element_size, int element_count);
bool pool_full(pool *p);
int pool_size(pool *p);
int pool_count(pool *p);
void pool_extend(pool *p, int new_count);
void *pool_get(pool *p);
void pool_release(pool *p, void *element);
void pool_destroy(pool *p);