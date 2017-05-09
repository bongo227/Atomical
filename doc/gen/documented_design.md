## Documented design

### Psudocode
This project was implemented in C which means their is a lot of code which is not important for an explanation on how each algorithm works. To help illustrate how the algorithms work I have opted to use psudocode so that it is easier to understand. The psudocode is close to the AQA psudocode spec with a few additions.

* `LABEL` and `ENDLABEL` make a labeled section of code
* `GOTO` goes to a label
* `_` is equivilent to `NULL` in C 

### String
Strings in C are represented by a pointer to some characters (that end with a null byte). This means that to append something to a string it would require a reallocation, which is slow. Additionally to find the length of a C string, it requires a function call which loops until it reaches the null byte unlike most languages were it would be a constant time operation. It makes sense in this case to build a more dynamic string for when we do not know how long the string should be, and do not care about the additional memory.

Strings have are split into two parts, the header and the characters. So that strings are compatible with the C standard library all the pointers returned by the string functions point to the characters so the header is hidden and must be accessed through the string functions. The header contains the length of the string (for fast direct access) and the capacity of the string (how many character can be stored before a reallocation must occur).

#### String equality
One side effect of knowing the strings length without looping though the string is faster string equality checks. The C standard library compares two strings character by character, if their is a difference the function returns false (non-zero) however since we have constant time access to the string length we can first check if the two strings lengths are equal thus improving performance in the case where two strings are not the same length.

```markdown
FUNCTION string_equals(string a, string b)
    length <- string_length(a)
    if length != string_length(b) THEN
        RETURN FALSE
    ELSE
        FOR i <- 0 TO length
            IF a[i] != b[i] THEN
                RETURN FALSE
            ENDIF
        LOOP
    ENDIF
    
    RETURN TRUE
ENDFUNCTION
```

#### string.h
This is the header file for the string implementation

```c
#pragma once

typedef struct {
    int length;
    int capacity;
} string_header;

typedef char *string;

string string_new(const char *str);
string string_new_length(const char *str, int len);
string string_new_file(FILE *f);

void string_free(string s);

string string_copy(string s);

int string_length(string s);
int string_capacity(string s);
int string_avalible(string s);

string string_expand(string s, int capacity);

string string_clear(string s);

string string_append(string s1, string s2);
string string_append_length(string s1, char *s2, int length);
string string_append_cstring(string s, char *str);

string string_slice(string s, int start, int end);

bool string_equals(string s1, string s2);
```

#### string.c
This is the source file for the string implementation

```c
#include "includes/string.h"

#define STRING_HEADER(s) ((string_header *)s - 1)

string string_new(const char *str) {
    int length = str ? strlen(str) : 0;
    return string_new_length(str, length);
}

string string_new_length(const char *str, int len) {
    // Create header + data
    void *data = malloc(sizeof(string_header) + len + 1);
    if (data == NULL) return NULL;
    
    string s = (char *)data + sizeof(string_header);
    
    // Set the header data
    string_header *header = STRING_HEADER(s);
    header->length = len;
    header->capacity = len;

    // Set the string data
    memcpy(s, str, len);
    s[len] = '\0';

    return s;
}

string string_new_file(FILE *f) {
    fseek (f, 0, SEEK_END);
	int file_length = ftell (f);
	fseek (f, 0, SEEK_SET);

    string s = string_new_length("", file_length);
    fread(s, 1, file_length, f);
    s[file_length] = '\0';
    STRING_HEADER(s)->length = file_length;

    return s;
}

void string_free(string s) {
    free(STRING_HEADER(s));
}

string string_copy(string s) {
    return string_new_length(s, string_length(s));
}

int string_length(string s) {
    return STRING_HEADER(s)->length;
}

int string_capacity(string s) {
    return STRING_HEADER(s)->capacity;
}

int string_avalible(string s) {
    string_header *header = STRING_HEADER(s);
    return header->capacity - header->length;
}

string string_expand(string s, int capacity) {
    string_header *header = STRING_HEADER(s);
    if (header->capacity > capacity) return s;
    header = realloc(header, sizeof(string_header) + capacity);
    header->capacity = capacity;
    return (char *)(header + 1);
}

string string_clear(string s) {
    return string_slice(s, 0, 0);
}

string string_append(string s1, string s2) {
    return string_append_length(s1, s2, string_length(s2));
}

string string_append_length(string s1, char *s2, int length) {
    int current_length = string_length(s1);
    s1 = string_expand(s1, current_length + length + 1);
    memcpy(s1 + current_length, s2, length);
    s1[current_length + length] = '\0';
    STRING_HEADER(s1)->length = current_length + length;
    return s1;
}

string string_append_cstring(string s, char *str) {
    return string_append_length(s, str, strlen(str));
}

string string_slice(string s, int start, int end) {
    string_header *header = STRING_HEADER(s);
    assert(start >= 0);
    assert(end <= header->length);
    int length = end - start;
    if(start > 0) memmove(s, s + start, length);
    s[length] = '\0';
    header->length = length;
    return s;
}

bool string_equals(string s1, string s2) {
    int length = string_length(s1);
    if (length != string_length(s2)) {
        return false;
    }

    for (int i = 0; i < length; i++) {
        if(s1[i] != s2[i]) return false;
    }

    return true;
}
```

### Pool
A compiler often allocates and frees many of the same structures. You could use the standard memory allocation function `malloc()` and `free()` however these are slow for many small allocations and the data has bad locality, so more cache misses occur when the data is read. The solution to this problem is a custom allocator, in this case a pool allocator.

A pool allocator takes a large chunk of memory and divides it into equal parts. When ever a piece of memory is required, the pool allocator returns a section of this memory. When the memory is freed the pool allocator reclaims this memory ready to give out on the next request.

#### Constructing a pool
The pool must first allocate a large chunk of memory to form the pool. From their the free list is set up by looping through each slice of memory and adding a header with pointer to the next slice of memory. 

```markdown
lastElement = memory[0]
FOR i = 0 TO elementCount
    element <- memory[i]
    lastElement.next <- element
    lastElement <- element
LOOP
lastElement.next <- _
```

Finally we create a structure to save some details about the pool.
```markdown
pool <- _
pool.memory = memory
pool.elementSize = elementSize
pool.elementCount = elementCount
pool.head = memory[0]
pool.tail = lastElement
RETURN pool
```

#### Pool full
If the pool is full then the free list is empty hence `pool.head` (and `pool.tail`) must be `NULL`

```markdown
FUNCTION poolFull(pool) 
    return pool.head = _
ENDFUNCTION
```

#### Pool count
For debugging purposes its often useful to know how much elements are in the pool. To compute this, we walk the free list to find out how much slices are free, then subtract that from `elementCount`

```markdown
FUNCTION poolCount(pool)
    IF poolFull(pool) THEN
        return pool.elementCount
    ELSE
        free <- 0
        ptr <- pool.head
        WHILE ptr != pool.tail 
            free <- free + 1
            ptr <- ptr.next
        ENDWHILE
        free <- free + 1
        
        RETURN pool.elementCount - free
    ENDIF
ENDFUNCTION
```

#### Expanding the pool
Expanding the pool is quite involved since the pools memory may have been moved by the reallocation. First we must save some details about the old pool.
```markdown
oldCount <- pool.count
isFull <- poolFull(pool)
```
Then we compute the pointer offset of the head and tail so that we can restore them later.
```markdown
oldMemory <- pool.memory
headOffset <- pool.head - oldMemory
tailOffset <- pool.tail - oldMemory
```
Now we can do the reallocation and restore the head and tail pointers.
```markdown
pool.memory <- realloc(pool.memory, pool.elementSize * pool.elementCount)
pool.head <- pool.memory + headOffset
pool.tail <- pool.memory + tailOffset
```
The free list pointers are also invalidated by the reallocation so they too must be restored.
```markdown
IF !isFull THEN
    ptr <- pool.head
    WHILE ptr != pool.tail
        ptr.next <- pool.memory + ptr.next - oldMemory
        ptr <- ptr.next
    ENDWHILE
ENDIF
```
Now that the pools capacity has expaned we need to expand the free list. First we set up a free list of all the new elements
```markdown
firstNew <- pool.memory[oldCount]
lastElement <- firstNew
FOR i = oldCount TO pool.elementCount
    element <- pool.memory[i]
    lastElement.next = element
    lastElement = element
LOOP
```
The two lists can now be joined unless the pool was full before, then their was not old list so the new list becomes the free list.
```markdown
IF isFull THEN
    pool.head <- firstNew
ELSE
    pool.tail.next <- firstNew
ENDIF
pool.tail <- lastElement
```
As you can see extending the pool is quite an expensive option, however the time it takes to (re)allocate a pool far outweighs the time taken to allocated many small structures.

#### Getting memory from the pool
If the pool is full then their is no memory to give, thus we expand the pool. Otherwise we return the next element in the free list, after removing it from the list.

```markdown
FUNCTION poolGet(pool)
    IF poolFull(pool) THEN
        poolExpand(pool, pool.elementCount * 2)
    ENDIF
    
    element <- pool.head
    pool.head <- pool.head.next
    RETURN element
ENDFUNCTIO
```

#### Releasing memory back to the pool
Releasing memory back into the pool is as simple as adding the slice back into the free list. If the pool is full then the element starts a new free list.
```markdown
FUNCTION poolRelease(pool, element)
    IF poolFull(pool) THEN
        pool.head <- element
        pool.head.next <- _
        pool.tail <- pool.head
    ELSE
        pool.tail.next <- element
        pool.tail <- element
    ENDIF
ENDFUNCTION
```

#### pool.h
This is the header file for the pool implementation
```c
#pragma once

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
```

#### pool.c
This is the source file for the pool implementation
```c
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

```

### Queue
Queues are an important are another important data structure which the C standard library doesn't provide, so I implemented my own. This queue uses a double ended linked list on the back end allowing for fast insertions and deletions from either end of the queue. This is not strictly a queue, and can also be used as a stack.

#### Queue push
To push onto a queue, the queue allocates a new element with a header which is returned to the caller for populating with data. To push to the front we add the new element at the start of the queue and link the new element and old element.

```markdown
FUNCTION queuePushFront(queue)
    queue.size <- queue.size + 1
    item <- malloc(queue.elementSize)
    
    item->prev <- _
    IF queue.first = _ THEN
        item.next <- _
        queue.last <- item
    ELSE
        item.next <- queue.first
        queue.first.prev <- item
    ENDIF
    queue.first <- item
    
    return item
ENDFUNCTION
```

Likewise to push to the back of the queue we use a simular routine, adding the new element at the end of the queue.

```markdown
FUNCTION queuePushBack(queue)
    queue.size <- queue.size + 1
    item <- malloc(queue.elementSize)
    
    item->next <- _
    IF queue.first = _ THEN
        item.prev <- _
        queue.first <- item
    ELSE
        item.prev <- queue.last
        queue.last.next <- item
    ENDIF
    queue.last <- item
    
    return item
ENDFUNCTION
```

#### Queue pop
Popping from the queue is as simple as removing an item from the front or back and updating the list.
```markdown
FUNCTION queuePopFrount(queue)
    IF queue.first = _
        RETURN _
    ENDIF
    
    q.size <- q.size - 1
    item <- queue.first
    
    IF q.first = q.last THEN
        q.last <- _
    ENDIF
    
    q.first.prev <- _
    q.first <- q.first.next
    
    RETURN item
ENDFUNCTION
```

```markdown
FUNCTION queuePopBack(queue)
    IF queue.last = _
        RETURN _
    ENDIF
    
    q.size <- q.size - 1
    item <- queue.last
    
    IF q.last = q.first THEN
        q.first <- _
    ENDIF
    
    q.last.next <- _
    q.last <- q.last.prev
    
    RETURN item
ENDFUNCTION
```

#### Queue destroy
Destroying the queue is as simple as walking along the linked list and freeing each node.

```markdown
FUNCTION queueDestroy(queue)
    WHILE queue.first != _
        next <- queue.first.next
        free(queue.first)
        queue.first <- next
    ENDWHILE
ENDFUNCTION
```

#### queue.h
This is the header file for the queue implementation

```c
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
```

#### queue.c
This is the source file for the queue implementation

```c
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

```

### Lexer
The lexer's job is to turn the source code into a sequence of tokens. A token is the smallest possible bit of meaningful source code such as a number or name. The following is all the possible types of token:

| Name | Example | Note |
| --- | --- | --- |
| ILLEGAL | | If a token isnt one of the other types of token it becomes an illegal token and somthing fately wrong has happened.
| END | | The last token, signifys the end of the token list. |
| IDENT | `foo` | Identifier |
| INT | `123` | Integer literal |
| FLOAT | `10.23` | Float literal |
| HEX | `0xFFFFFF` | Hex literal |
| OCTAL | `0123` | Octal literal |
| STRING | `"foo"` | String literal |
| BREAK | `break` | Break keyword |
| CASE | `case` | Case keyword |
| CONST | `const` | Const keyword |
| CONTINUE | `continue` | Continue keyword |
| DEFAULT | `default` | Default keyword |
| ELSE | `else` | Else keyword |
| FALLTHROUGH | `fallthrough` | Fallthrough keyword |
| FOR | `for` | For keyword |
| FUNC | `func` | Func keyword |
| PROC | `proc` | Proc keyword |
| IF | `if` | If keyword |
| IMPORT | `import` | Import keyword |
| RETURN | `return` | Return keyword |
| SELECT | `select` | Select keyword |
| STRUCT | `struct` | Struct keyword |
| SWITCH | `switch` | Switch keyword |
| TYPE | `type` | Type keyword |
| VAR | `var` | Var keyword |
| DEFER | `defer` | Defer keyword |
| DEFINE | `:=` | Define symbol |
| SEMI | `;` | Semicolon symbol |
| COLON | `:` | Colon symbol |
| DOUBLE_COLON | `::` | Double colon symbol |
| ELLIPSE | `...` | Ellipse symbol |
| PERIOD | `.` | Period symbol |
| COMMA | `,` | Comma symbol |
| LPAREN | `(` | Left parentesis symbol |
| RPAREN | `)` | Right parentesis symbol |
| LBRACK | `[` | Left bracket symbol |
| RBRACK | `]` | Right parentesis symbol |
| LBRACE | `{` | Left brace symbol |
| RBRACE | `}` | Right brace symbol |
| ADD | `+` | Add symbol |
| ADD_ASSIGN | `+=` | Add assign symbol |
| INC | `++` | Increment symbol |
| ARROW | `->` | Arrow symbol |
| SUB | `-` | Substract symbol |
| SUB_ASSIGN | `-=` | Subtract assign symbol |
| DEC | `--` | Decrement symbol |
| MUL | `*` | Multiplication symbol |
| MUL_ASSIGN | `*=` | Mutltiply assign symbol |
| QUO | `/` | Quotiant symbol |
| QUO_ASSIGN | `/=` | Quotiant assign symbol |
| REM | `%` | Remainder symbol |
| REM_ASSIGN | `%=` | Remainder assign symbol |
| XOR | `^` | Excusive or symbol |
| XOR_ASSIGN | `^=` | Excusive or assign symbol |
| GTR | `>` | Greater than symbol |
| GEQ | `>=` | Greater than or equal to symbol |
| LSS | `<` | Less than symbol |
| LEQ | `<=` | Less than or equal to symbol |
| SHL | `<<` | Shift left symbol |
| SHL_ASSIGN | `<<=` | Shift left assign symbol |
| SHR | `>>` | Shift right symbol |
| SHR_ASSIGN | `>>=` | Shift right assign symbol |
| ASSIGN | `=` | Assign symbol |
| EQL | `==` | Equality symbol |
| NOT | `!` | Not symbol |
| NEQ | `!=` | Not equal to symbol |
| AND | `&` | Bitwise and sybol |
| AND_ASSIGN | `&=` | Bitwise and assign symbol |
| AND_NOT | `&^` | Bit clear symbol |
| AND_NOT_ASSIGN | `&^=` | Bit clear assign symbol |
| LAND | `&&` | Logical and symbol |
| OR | `\|` | Bitwise or symbol |
| OR_ASSIGN | `\|=` | Bitwise or assign symbol |
| LOR | `\|\|` | Logical or symbol |


#### Lexing
To convert the source to token the lexer runs through the file character by character invoking different procedures depending on which character the head points to. For example when a `"` is at the read pointer we know its the start of a string literal which end when we reach the closing `"` character. In psudocode this looks like the following.

```markdown
FUNCTION lex(readPointer) 
    tokens <- []
    
    WHILE readPointer != _ 
        SWITCH readPointer
            ...
            CASE '"':
                type <- STRING
                value <- ""
                next(readPointer)
                WHILE readPointer != `"`
                    value <- value + readPointer
                ENDWHILE
            ...
        ENDSWITCH
        
        tokens <- Token{type, value}
    ENDWHILE
ENDFUNCTION
```

#### Semicolon insertion
In C every statement ends in a semi-colon. This tells the compiler that it is the end of a statement, and that the next tokens form a separate statement. Most code only has a single statement per line so almost all semi-colons are at the end of a line, thus if the compiler was to insert semicolons at the end of all lines we could emit semicolons from the source code as in languages like Go and JavaScript. 

To insert semi-colons in the correct place a simple `semi` flag tells the compiler weather to insert a semi at the end of the line. If this flag is false `clearWhitespace` (which moves the `readPointer` past all the white space characters) skips over the newline without inserting a semi colon, else the `\n` character is detected in the lex routine and a semi colon is inserted in its place.

```markdown
FUNCTION clearWhitespace()
    WHILE readPointer = ' ' OR 
        readpointer = '\t' OR 
        (readpointer = '\n' AND !semi) OR
        readpointer = '\r'
        
        next(readPointer)
        column -> column + 1
    ENDWHILE
ENDFUNCTION
```

#### Extracting a number
Fur has four types of numerical literals: int's, floats, octal's and hexadecimals. Instead of a separate procedure for each one which would require backtracking/look-head and a lot of code duplication we use a single procedure. Note this uses goto's which are normally a terrible idea, in this case they make the code more efficient, cleaner and easier to follow. 

The first part is `extractMantissa` which returns all the digits which are less than `base`. `isDigit` checks if a character literal is a digit (0-F) and `asDigit` converts a character literal to an integer. `character` is the current character the lexer is reading and `nextCharacter` moves the `character` pointer to the next character.  

```markdown
FUNCTION extractMantissa(base)
  mantissa <- ""
  WHILE isDigit(character) && asDigit(character) < base THEN
    mantissa <- mantissa + character
    nextCharacter()
  LOOP
  RETURN mantissa
END FUNCTION
```

This is the main part of the algorithm. If first checks for hexadecimal's or octal's then integers and floats, when we check for octal's we may find that it was actually an integer (or float) in which case we can jump to the appropriate point. 
```markdown
FUNCTION number()
  number <- ""
  type <- INTEGER
  base <- 10

  IF character = '0' THEN
    nextCharacter()
    IF character = 'x' OR character = 'X' THEN
      type <- HEXEDECIMAL
      base <- 16
      nextCharacter()
      number <- number + extractMantissa(base)
    ELSE
      octal <- TRUE
      base <- 8
      number <- number + extractMantissa(base)

      IF character = '8' OR character = '9' THEN
        octal <- FALSE
        base <- 10
      ELSE
        type <- OCTAL
      END IF

      IF character = '.' THEN
        GOTO fraction
      END IF

      IF NOT octal THEN
        ERROR "Illegal octal number"
      END IF
    END IF

    GOTO exit
  END IF

  base <- 10
  number <- number + extractMantissa(base)

LABEL fraction
  IF character = '.' THEN
    number <- number + '.'
    nextCharacter()
    type <- FLOAT
    base <- 10
    number <- number + extractMantissa(base)
  END IF
END LABEL

LABEL exit
  RETURN number
END LABEL

END FUNCTION
```

#### lexer.h
This is the header file for the lexer implementation

```c
#pragma once

#include "all.h"
#include <string.h>
#include <stdlib.h>

typedef enum {
	ILLEGAL,
	IDENT,
	
	// literals
	INT,
	FLOAT,
	HEX,
	OCTAL,
	STRING,

	// key words
	BREAK,
	CASE,
	CONST,
	CONTINUE,
	DEFAULT,
	DEFER,
	ELSE,
	FALLTHROUGH,
	FOR,
	FUNC,
	PROC,
	IF,
	IMPORT,
	RETURN,
	SELECT,
	STRUCT,
	SWITCH,
	TYPE,
	VAR,

	// symbols
	SEMI,
	COLON,
	DEFINE,
	DOUBLE_COLON,
	ELLIPSE,
	PERIOD,
	COMMA,
	LPAREN,
	RPAREN,
	LBRACK,
	RBRACK,
	LBRACE,
	RBRACE,
	ADD,
	ADD_ASSIGN,
	INC,
	ARROW,
	SUB,
	SUB_ASSIGN,
	DEC,
	MUL,
	MUL_ASSIGN,
	QUO,
	QUO_ASSIGN,
	REM,
	REM_ASSIGN,
	XOR,
	XOR_ASSIGN,
	LSS,
	LEQ,
	SHL,
	SHL_ASSIGN,
	GTR,
	GEQ,
	SHR,
	SHR_ASSIGN,
	ASSIGN,
	EQL,
	NOT,
	NEQ,
	AND_NOT,
	AND_NOT_ASSIGN,
	AND,
	AND_ASSIGN,
	LAND,
	OR,
	OR_ASSIGN,
	LOR,
	
	// eof token
	END,
} TokenType;

typedef struct {
	TokenType type;
	int line;
	int column;
	char *value;
} Token;

typedef struct {
	char *source;
	int line;
	int column;
	bool semi;
} Lexer;

Token *Lex(char *source);
char *TokenName(TokenType type);
char *GetLine(char *src, int line);
int get_binding_power(TokenType type);
```

#### lexer.c
This is the source file for the lexer implementation

```c
#include "includes/lexer.h"

// Removes spaces, newlines and tabs
void clearWhitespace(Lexer *lexer) {
	while (*lexer->source == ' ' ||
		*lexer->source == '\t' ||
		(*lexer->source == '\n' && !lexer->semi) ||
		*lexer->source == '\r') {

		lexer->source++;
		lexer->column++;
	}
}

// checks if the character is a digit 0-9
bool isDigit(char *input) {
	return *input >= '0' && *input <= '9';
}

// checks if the character is a letter a-z or A-Z
bool isLetter(char *input) {
	return (*input >= 'a' && *input <= 'z') || 
		(*input >= 'A' && *input <= 'Z');
}

// returns the character as an integer
int asDigit(char *input) {
	if ('0' <= *input && *input <= '9') {
		return *input - '0';
	} else if('a' <= *input && *input <= 'f') {
		return *input - 'a' + 10;
	} else if('A' <= *input && *input <= 'F') {
		return *input - 'A' + 10;
	}

	return -1; // unrecognised digit
}

// returns the word at the start of the string
char *word(Lexer *lexer) {
	// token is an ident
	char *word = (char *)malloc(1024);

	// copy all letter characters
	char *wordPtr = word;
	int length = 0;
	do {
		*wordPtr++ = *lexer->source;
		lexer->source++;
		lexer->column++;
		length++;
	} while (isLetter(lexer->source) || isDigit(lexer->source));

	// null-terminate
	*wordPtr = '\0';
	length++;

	// shrink string to ident length
	word = realloc(word, length);

	return word;
}

// Extracts the mantiass from input into number, returning the characters written.
int extractMantissa(char **input, char **number, int base) {
	int length = 0;
	while(isDigit(*input) && asDigit(*input) < base) {
		**number = **input;
		length++;
		(*input)++;
		(*number)++;
	}

	return length;
}

// extracts the number at the start of the input string, returns error code
char *number(Lexer *lexer, TokenType *type) {
	char *number = (char *)malloc(1024 * sizeof(char));
	char *numberPtr = number;
	
	*type = INT;
	int base = 10;
	int length = 0; //amount of characters in number

	if (*lexer->source == '0') {
		lexer->source++;
		if (*lexer->source == 'x' || *lexer->source == 'X') {
			// number is hexadecimal
			*type = HEX;
			base = 16;
			lexer->source++; // skip 'x' / 'X'
			length += extractMantissa(&lexer->source, &numberPtr, base);
		} else {
			// assume number is octal
			bool octal = true;
			base = 8;
			length += extractMantissa(&lexer->source, &numberPtr, base);
			
			if (*lexer->source == '8' || *lexer->source == '9') {
				// number was not octal
				octal = false;
				base = 10;
				length += extractMantissa(&lexer->source, &numberPtr, base);
			}
			else {
				*type = OCTAL;
			}

			if (*lexer->source == '.') {
				// number is fractional
				goto fraction;
			}

			if (!octal) {
				// illegal octal number
				return "";
			}
		}
		goto exit;
	}

	base = 10;
	length += extractMantissa(&lexer->source, &numberPtr, base);

fraction:
	if (*lexer->source == '.') {
		// copy '.'
		*numberPtr++ = '.';
		lexer->source++; 
		length++;
		
		*type = FLOAT;
		base = 10;
		length += extractMantissa(&lexer->source, &numberPtr, base);
	}

exit:
	// Null terminate
	length++;
	*numberPtr = '\0';

	number = realloc(number, length * sizeof(char));
	return number;
}

char *escape(char **input, char quote) {
	char *esc = (char *)malloc(1024 * sizeof(char));
	int length = 0;
	int n = 0;
	int base = 0;
	int max = 0;

	// copy '/'
	*esc++ = '/';
	length++;
	(*input)++;

	if (**input == quote) {
		(*input)++;
	}
	 
	switch (**input) {
	case 'a':
	case 'b': 
	case 'f': 
	case 'n':
	case 'r':
	case 't':
	case 'v':
	case '\\':
		*esc++ = **input;
		length++;
		(*input)++;
		break;
	
	// octal
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		n = 3;
		base = 8;
		max = 255;
		break;

	// hex
	case 'x':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 2;
		base = 16;
		max = 255;
		break;

	// small unicode
	case 'u':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 4;
		base = 16;
		max = 0x0010FFFF;
		break;

	// full unicode
	case 'U':
		*esc++ = **input;
		length++;
		(*input)++;
		n = 8;
		base = 16;
		max = 0x0010FFFF;
		break;

	default:
		// unkown escape sequence
		break;

	}

	int x = 0;
	while (n > 0) {
		int d = asDigit(*input);
		if (d >= base) {
			// illegal character
		}

		x = x * base + d;
		*esc++ = **input;
		length++;
		(*input)++;
		n--;
	}

	// check if unicode character is valid
	if (x > max || (0xD800 <= x && x < 0xE000)) {
		// invalid unicode code point
	}

	// null terminate
	*esc = '\0';
	length++;

	esc = realloc(esc, length * sizeof(char));
	return esc;
}

char *lex_string(char **input) {
	char *string = (char *)malloc(1024 * sizeof(char));
	int length = 0;

	(*input)++; // skip '"'
	char *stringPtr = string;
	while (**input != '"') {
		if (**input == '\\') {
			char *esc = escape(input, '"');
			while (*esc) {
				*stringPtr++ = *esc++;
				length++;
			}
		}

		*stringPtr++ = **input;
		length++;
		(*input)++;
	}
	(*input)++; // skip '"'
	
	// null terminate
	*stringPtr = '\0';
	length++;

	string = realloc(string, length * sizeof(char));
	return string;
}

TokenType switch2(char **input, TokenType token0, TokenType token1) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	return token0;
}

TokenType switch3(char **input, TokenType token0, TokenType token1, char chr, TokenType token2) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	if (**input == chr) {
		(*input)++;
		return token2;
	} 
	return token0;
}

TokenType switch4(char **input, TokenType token0, TokenType token1, char chr, TokenType token2, TokenType token3) {
	(*input)++;
	if (**input == '=') {
		(*input)++;
		return token1;
	}
	if (**input == chr) {
		(*input)++;
		if (**input == '=') {
			(*input)++;
			return token3;
		}	
		return token2;
	} 
	return token0;
}

// returns the second character in a string
char peek(char *input) {
	return *(++input);
}

void next(Lexer *lexer) {
	lexer->source++;
	lexer->column++;
}

// finds the token type for a word
TokenType keyword(char *word) {
	if (strcmp(word, "break") == 0) return BREAK;
	if (strcmp(word, "case") == 0) return CASE;
	if (strcmp(word, "const") == 0) return CONST;
	if (strcmp(word, "continue") == 0) return CONTINUE;
	if (strcmp(word, "default") == 0) return DEFAULT;
	if (strcmp(word, "defer") == 0) return DEFER;
	if (strcmp(word, "else") == 0) return ELSE;
	if (strcmp(word, "fallthrough") == 0) return FALLTHROUGH;
	if (strcmp(word, "for") == 0) return FOR;
	if (strcmp(word, "func") == 0) return FUNC;
	if (strcmp(word, "proc") == 0) return PROC;
	if (strcmp(word, "if") == 0) return IF;
	if (strcmp(word, "import") == 0) return IMPORT;
	if (strcmp(word, "return") == 0) return RETURN;
	if (strcmp(word, "select") == 0) return SELECT;
	if (strcmp(word, "struct") == 0) return STRUCT;
	if (strcmp(word, "switch") == 0) return SWITCH;
	if (strcmp(word, "type") == 0) return TYPE;
	if (strcmp(word, "var") == 0) return VAR;
	return IDENT;
}

Token *Lex(char *source) {
	Lexer lexer = {source, 1, 1, false};

	Token *tokens = (Token *)malloc(0);

	int i = 0;
	while (*lexer.source) {
		i++;
		clearWhitespace(&lexer);

		Token token;
		token.line = lexer.line;
		token.column = lexer.column;
		token.value = "";

		if (isLetter(lexer.source)) {
			// token is an identifier
			token.value = word(&lexer);
			token.type = keyword(token.value);
			if (token.type == IDENT || 
				token.type == BREAK || 
				token.type == CONTINUE || 
				token.type == FALLTHROUGH || 
				token.type == RETURN) {
			
				lexer.semi = true;
			}
		}
		else if (isDigit(lexer.source)) {
			// token is a number
			lexer.semi = true;
			token.type = INT;
			token.value = number(&lexer, &token.type);
		} else {
			// token is a symbol
			switch (*lexer.source) {
				case '\n':
					token.type = SEMI;
					lexer.semi = false;
					lexer.column = 1;
					lexer.line++;
					next(&lexer);
					break;

				case '"':
					lexer.semi = true;
					token.type = STRING;
					token.value = lex_string(&lexer.source);
					break;

				case ':':
					token.type = switch3(&lexer.source, COLON, DEFINE, ':', DOUBLE_COLON);
					break;

				case '.':
					token.type = PERIOD;
					next(&lexer);
					if (*lexer.source == '.') {
						next(&lexer);
						if (*lexer.source == '.') {
							next(&lexer);
							token.type = ELLIPSE;
						}
					}
					break;

				case ',':
					token.type = COMMA;
					next(&lexer);
					break;

				case ';':
					token.type = SEMI;
					next(&lexer);
					break;

				case '(':
					token.type = LPAREN;
					next(&lexer);
					break;

				case ')':
					lexer.semi = true;
					token.type = RPAREN;
					next(&lexer);
					break;

				case '[':
					token.type = LBRACK;
					next(&lexer);
					break;

				case ']':
					lexer.semi = true;
					token.type = RBRACK;
					next(&lexer);
					break;

				case '{':
					lexer.semi = false;
					token.type = LBRACE;
					next(&lexer);
					break;

				case '}':
					lexer.semi = true;
					token.type = RBRACE;
					next(&lexer);
					break;

				case '+':
					token.type = switch3(&lexer.source, ADD, ADD_ASSIGN, '+', INC);
					if (token.type == INC) lexer.semi = true;
					break;

				case '-':
					if (peek(lexer.source) == '>') {
						token.type = ARROW;
						lexer.source += 2;
					} else {
						token.type = switch3(&lexer.source, SUB, SUB_ASSIGN, '-', DEC);
						if (token.type == DEC) lexer.semi = true;
					}
					break;

				case '*':
					token.type = switch2(&lexer.source, MUL, MUL_ASSIGN);
					break;

				case '/':
					token.type = switch2(&lexer.source, QUO, QUO_ASSIGN);
					break;

				case '%':
					token.type = switch2(&lexer.source, REM, REM_ASSIGN);
					break;

				case '^':
					token.type = switch2(&lexer.source, XOR, XOR_ASSIGN);
					break;

				case '<':
					token.type = switch4(&lexer.source, LSS, LEQ, '<', SHL, SHL_ASSIGN);
					break;

				case '>':
					token.type = switch4(&lexer.source, GTR, GEQ, '>', SHR, SHR_ASSIGN);
					break;

				case '=':
					token.type = switch2(&lexer.source, ASSIGN, EQL);
					break;

				case '!':
					token.type = switch2(&lexer.source, NOT, NEQ);
					break;

				case '&':
					if (peek(lexer.source) == '^') {
						next(&lexer);
						token.type = switch2(&lexer.source, AND_NOT, AND_NOT_ASSIGN);
					} else {
						token.type = switch3(&lexer.source, AND, AND_ASSIGN, '&', LAND);
					}
					break;

				case '|':
					token.type = switch3(&lexer.source, OR, OR_ASSIGN, '|', LOR);
					break;

				default:
					token.type = ILLEGAL;
					next(&lexer);
					break;
			}
		}

		if (token.value == NULL) token.value = TokenName(token.type);

		// Add the token to the array
		tokens = (Token *)realloc(tokens, (i + 1) * sizeof(Token));
		tokens[i - 1] = token;

		// Check for newline
		if (*lexer.source == '\n') {
			lexer.line++;
		}

	}

	// End of file token
	Token token;
	token.column = 321;
	token.line = 321;
	token.value = "";
	token.type = END;
	tokens[i] = token;

	return tokens;
}

char *TokenName(TokenType type) {
	switch(type) {
		case ILLEGAL: return "illegal";
		case END: return "[END]";
		case IDENT: return "[ident]";
		
		case INT: return "[int]";
		case FLOAT: return "[float]";
		case HEX: return "[hex]";
		case OCTAL: return "[octal]";
		case STRING: return "[string]";
		
		case BREAK: return "break";
		case CASE: return "case";
		case CONST: return "const";
		case CONTINUE: return "continue";
		case DEFAULT: return "default";
		case ELSE: return "else";
		case FALLTHROUGH: return "fallthrough";
		case FOR: return "for";
		case FUNC: return "func";
		case PROC: return "proc";
		case IF: return "if";
		case IMPORT: return "import";
		case RETURN: return "return";
		case SELECT: return "select";
		case STRUCT: return "struct";
		case SWITCH: return "switch";
		case TYPE: return "type";
		case VAR: return "var";
		case DEFER: return "defer";

		case DEFINE: return ":=";
		case SEMI: return ";";
		case COLON: return ":";
		case DOUBLE_COLON: return "::";
		case ELLIPSE: return "...";
		case PERIOD: return ".";
		case COMMA: return ",";
		case LPAREN: return "(";
		case RPAREN: return ")";
		case LBRACK: return "[";
		case RBRACK: return "]";
		case LBRACE: return "{";
		case RBRACE: return "}";
		case ADD: return "+";
		case ADD_ASSIGN: return "+=";
		case INC: return "++";
		case ARROW: return "->";
		case SUB: return "-";
		case SUB_ASSIGN: return "-=";
		case DEC: return "--";
		case MUL: return "*";
		case MUL_ASSIGN: return "*=";
		case QUO: return "/";
		case QUO_ASSIGN: return "/=";
		case REM: return "%";
		case REM_ASSIGN: return "%=";
		case XOR: return "^";
		case XOR_ASSIGN: return "^=";
		case GTR: return ">";
		case GEQ: return ">=";
		case LSS: return "<";
		case LEQ: return "<=";
		case SHL: return "<<";
		case SHL_ASSIGN: return "<<=";
		case SHR: return ">>";
		case SHR_ASSIGN: return ">>=";
		case ASSIGN: return "=";
		case EQL: return "==";
		case NOT: return "!";
		case NEQ: return "!=";
		case AND: return "&";
		case AND_ASSIGN: return "&=";
		case AND_NOT: return "&^";
		case AND_NOT_ASSIGN: return "&^=";
		case LAND: return "&&";
		case OR: return "|";
		case OR_ASSIGN: return "|=";
		case LOR: return "||";
	}

	return "UNKOWN_NAME";
}

char *GetLine(char *source, int line) {
	int currentLine = 1;

	while(*source != '\0') {
		if (*(source++) == '\n') currentLine++;
		
		if (currentLine == line) {
			// find the line length
			int lineLength = 0;
			while(*source && *source != '\n') {
				source++;
				lineLength++;
			}

			// copy the line to a buffer and return
			char *buff = malloc((lineLength+1) * sizeof(char));
			memcpy(buff, source - lineLength, lineLength);
			buff[lineLength] = '\0';
			return buff;
		}
	}

	return "LINE_NOT_FOUND";
}

// get_binding_power returns the left binding power of a token
int get_binding_power(TokenType type) {
	switch (type) {
	case END:
		return -10;
	// Non-binding operators
	case SEMI:
		return 0;
	// Assignment operators
	case ASSIGN:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MUL_ASSIGN:
	case REM_ASSIGN:
	case OR_ASSIGN:
	case SHR_ASSIGN:
	case DEFINE:
		return 10;
	// Logical operators
	case LAND:
	case LOR:
		return 20;
	// Equality operators
	case EQL:
	case NEQ:
	case LSS:
	case GTR:
	case LEQ:
	case GEQ:
		return 30;
	// Math operators
	case ADD:
	case SUB:
		return 40;
	case MUL:
	case QUO:
	case REM:
		return 50;
	// Special unary
	case NOT:
		return 60;
	// Strongly bound
	case PERIOD:
	case LBRACK:
	case LPAREN:
		return 70;
	// Unknow token
	default:
		return 0;
	}
}
```

### Parser
The parser takes the list of tokens constructed by the lexer and transforms them into an abstract syntax tree, which is a tree structure which represents the program being compiled. An AST is constructed from nodes each with children for example the expression -a + 4 * 5 would be parsed into:
```go
BinaryNode {
	Left: UnaryNode {
		Operator: '-'
		Expression:  IdentNode{
			name: 'a'
		}
	}
	Operator: '+'
	Right: BinaryNode {
		Left: LiteralNode{
			value: 4
		}
		Operator: '*'
		Right: LiteralNode {
			value: 5
		}
	}
}
```

This structure allows us to more easily translate the higher level language into a lower level one.

Nodes are slit into 3 types, expressions, statements and declarations.

#### Expressions
Expression nodes are nodes that can be evaluated.

| Name | Example | Notes |
| ---- | ------- | ----- |
| identExp | `foo` | An identifier |
| literalExp | `123` | Any literal value including numbers and strings |
| unaryExp | `-100` | An expression whith a leading unary operation |
| binaryExp | `10 + 3` | Binary expression with an infix operation |
| selectorExp | `foo.bar` | A selection expression, for accessing keys in a struct |
| indexExp | `foo[0]` | An index expression, for accessing items in an array |
| callExp | `foo(bar, baz)` | Function call expression |
| keyValueExp | `foo: 123` | Expression with a (optional key) and value |
| keyValueListExp | `{foo: 123, bar: 321}` | List of key value expressions |
| structValueExp | `foo{bar: 123}` | Key value list with a procedding type |
| arrayExp | `[1, 2, 3]` | Array initilizer expression |
| arrayType | `int[10]` | Array type expression |

#### Statements
Statements provide the control flow and assignment syntax.

| Name | Example | Notes |
| ---- | ------- | ----- |
| declareSmt | `foo := 100` | Wrapper around declare node |
| assignmentSmt | `foo = 100` | Varible assignment |
| retSmt | `return 100` | Return statement |
| ifSmt | `if foo > 19 {}` | If statement |
| forSmt | `for i := 0; i < 10; i++ {}` | For statement |

#### Declarations
Declare nodes are the top level parent nodes.

| Name | Example | Notes |
| --- | --- | --- |
| argumentDcl | `int a` | Function argument declaration |
| functionDcl | `proc foo :: int bar, int baz -> int` | Function declaration |
| varibleDcl | `var int a = 100` | Varible declarations |

#### Pratt Parser

When it comes to language parsing their are many different options. A parser generator can automatically build a parser from a list of rules, however the error messages that they produce can be hard to understand and not very customizable. For this reason most languages opt to right their own parser from scratch as I did with Fur. For this project I implemented a Top down operator precedence parser, also known as a Pratt parser, with a few modifications.

The key parts of this algorithm is the `led` and `nud` procedures. `led` (short for left denotation) parses the next token in an infix context i.e. the token joins two expressions. The original Pratt parser was only designed for expressions however I have added a a `stmd` function which parses a statement in a similar style.

##### Nud method
`nud` (short for null denotation) parser a token in a prefix context, i.e. the token starts a new expression.

```markdown
FUNCTION nud(token)
	SWITCH typeof(token)
		# Case statements here ...
	END SWITCH
END FUNCTION
```
###### Literal expression
If a literal is at the beginning of an expression (or sub-expression) we return a literal node, any following operators will be handled in an infix context. This is a key distinction between some other algorithms, by delaying infix operators we are able to parse mathematical expressions like a + b * c without having to use something like the shunting yard algorithm which requires allocating a stack (two stacks for function calls) and queue.

```markdown
CASE INT, FLOAT, HEX, OCTAL, STRING:
	return LiteralNode { token }
```

###### Ident expression
```markdown
CASE IDENT:
	return IdentNode { token }
```

###### Unary expression
A unary node is normally something like `-14`, the operator in front changes the value to the right. To parse a unary node we treat the tokens after the operator as a sub-expression by calling `expression` (see below). 

We parse in a value of 60 for the binding power of the sub expression, this is the other unique feature of a Pratt parser. To delay the parsing of infix tokens we use the concept of binding power, which is how well tokens should _bind_ to each other. For example the binding power of `*` will be higher than the binding power of `-` since we always multiply before subtracting. By calling `expression` we get the next sub expression where all the tokens are bound by a binding power higher than the right binding power (which is the parameter to expression).

`expression` is what gives the ast its tree structure and allows us to parse expressions such as `-(14 + a)`, in this case the expression on the right wasnt a simple literal value but a binary expression.

```markdown
CASE NOT, SUB:
	return UnaryNode { token, expression(60) }
```

###### Key value expression
A key value node is a list of values with keys such as:
```markdown
{
    baz: 123,
    qux: 2.4,
}
```
but the keys may be ommited 
```markdown
{ 123, 2.4 }
```
To parse a list of values we use a loop to parse each key value pair until we reach a `}` which is the end of list. Inside the loop we try to parse a key or a value, since we do not know if the item has a key we must check if the next token is a `:`. If it is then the `keyOrValue` was a key and the value expression is parsed, if their was no colon then `keyOrValue` must be a value. A `KeyValue` node is then appended to the `keyValues` array which will form the `KeyValueList` node. 

```markdown
CASE LPAREN:
    count <- 0
    keyValues <- []
    
    WHILE token != RBRACE DO
        keyOrValue <- expression()
        key <- NULL
        value <- NULL

        if token == COLON THEN
            nextToken()
            key <- keyOrValue
            value = expression()
        ELSE
            value = keyOrValue
        ENDIF
        
        keyValues[count] <- KeyValue {key, value}
        count <- count + 1
        
        expect(COMMA)
    ENDWHILE
    
    RETURN KeyValueList{keyValues}
```

###### Array expression
Array nodes are very similar to a `KeyValueList` expression however they can omit the check for keys since array initialiser does not have keys

```markdown
CASE LPAREN:
    count <- 0
    values <- []
    
    WHILE token != RBRACK DO
        value <- expression()
        values[count] <- value
        count <- count + 1
        expect(COMMA)
    ENDWHILE
    
    RETURN Array{keyValues}
```

##### Led method
The led method parses a token that in an infix context, i.e. between two nodes.
```markdown
FUNCTION led(exp, token)
    rbp <- getBindingPower(token)
    SWITCH typeof(token)
        # Case statements here ...
    END SWITCH
END FUNCTION
```

###### Binary expression
A binary expression is like a unary expression except the operator is in the middle of two values such as `41 + 2`. In this case `41` would have already been parsed and `+` is the current token. To finish the node the right hand side must be constructed by a call to `expression`.

Assignment statements are also parsed here and then transformed into the correct node elsewhere otherwise some look ahead functionality would be required which would make the parser less efficient.

```markdown
CASE ADD, SUB, MUL, QUO, REM, 
    EQL, NEQ, GTR, LSS, GEQ, LEQ, LAND, LOR,
    ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, REM_ASSIGN, OR_ASSIGN, SHL_ASSIGN:
    
    RETURN BinaryExp{exp, token, expression(rbp)}
```

###### Selector expression
A selector expression is exactly the same as a binary expression except the two values are separated by a `.`.

```markdown
CASE PERIOD:
    RETURN SelectorExp{exp, expression(rbp)}
```

###### Call expression
A call expression is similar to a key value list expression.

```markdown
CASE LPAREN:
    count <- 0
    parameters <- []
    
    WHILE token != RPAREN DO
        param <- expression()
        parameters[count] <- param
        count <- count + 1
        expect(COMMA)
    ENDWHILE
    
    RETURN Call{parameters, exp}
```

##### Smtd method
Statement parses the token in the context of the start of a _statement_, instead of a expression as in `nud`. This is an important distinction since the same token can mean different things if it is at the start of an expression of statement.

###### Return statement
Return statements are simple nodes with a single expression being the value the statement returns.
```markdown
CASE RETURN:
    RETURN Return{expression()}
```

###### Block statement
Block statements are used in more complex statements such as if and for statments or on their own.
```markdown
CASE LBRACE:
    expect(LBRACE)
    smtCount <- 0
    smts <- []
    WHILE token != RBRACE THEN
        smtCount <- smtCount + 1
        smts[smtCount] <- statement()
        expect(SEMI)
    ENDWHILE
    expect(RBRACE)
    RETURN Block{smts}
```

###### If statement
To parse an if statements we look for the `IF` token, then parse the condition as an expression and the block as a block. If their is an `ELSE` after the block, we check for an `IF`. If their is an `IF` we recursively call `if()` to parse the next if, otherwise it must be a final else with no condition.

```markdown
CASE IF:
    expect(IF)
    condition <- expression()
    block <- block()
    else <- _
    IF token == ELSE THEN
        expect(ELSE)
        IF token == IF THEN
            else <- if()
        ELSE
            else <- If{_, block(), _}
        ENDIF
    ENDIF
    
    RETURN If{condition, block, else}
```

###### For statement
For statements are similar to if statements

```markdown
CASE FOR:
    expect(FOR)
    index <- declaration()
    condition <- expression()
    body <- block()
```

###### Variable declaration
Variable declarations are normally handled elsewhere however if it starts with the optional `var` then we can parse the variable declaration now (rather than later).

```markdown
CASE VAR:
    RETURN declaration()
```

###### Increment expression
Increment statements do not start with a keyword, so we look for an identifier. If their is not a proceeding increment or decrement token then we know the expression is a assignment or declaration node so we let the caller handle it.

```markdown
CASE IDENT:
    ident <- Ident()
    SWITCH token
        CASE INC:
            RETURN binary{ident, ADD_ASSIGN, 1}
        CASE DEC:
            RETURN binary{ident, SUB_ASSIGN, 1}
    ENDSWITCH
    RETURN _
```

##### Declarations
Their are only two declaration nodes, functions and variables.

###### Function declaration
Function declaration are more complicated than most nodes since they contain a lot of information

```markdown
CASE PROC:
    expect(PROC)
    name <- Ident()
    expect(DOUBLE_COLON)
    
    args <- []
    argCount <- 0
    WHILE token != ARROW OR token != LBRACE
        type <- Type()
        name <- Ident()
        args[argCount] = Arg{type, name}
        argCount <- argCount + 1
    ENDWHILE
    
    expect(ARROW)
    
    returnType <- Type()    
    
    body <- Block()
    
    RETURN Function{name, args, returnType, body}
```

###### Variable declaration
Variable declaration come in two forms, short form such as `foo := 123` and `var bar int = 199`. In the case of the long form declaration we know ahead of time that is a variable declaration since it starts with the var keyword.

```markdown
CASE VAR:
    type <- Type()
    name <- Ident()
    value <- Expression()
    RETURN VariableDcl{type, name, value}
```

Otherwise the statement wrapper function first try's to parse it as a statement with `smtd()`, for short variable declarations this will return `NULL`. Next it try's parsing it as an expression, this will return a binary expression node where the operator is a assign or define type. From their the expression is converted into a statement.

```markdown
FUNCTION Statement()
    statement <- smtd()
    IF statement != _ THEN
        RETURN statement
    ENDIF
    
    expression <- Expression()
    IF expression = _ THEN
        RETURN _
    ENDIF
    
    SWITCH expression.op
        CASE ASSIGN, ADD_ASSIGN, SUB_ASSIGN, MUL_ASSIGN, REM_ASSIGN, OR_ASSIGN, SHL_ASSIGN:
            RETURN Assignment{expression.left, expression.op, expression.right}
        CASE DEFINE:
            RETURN VariableDcl{expression.left, expression.right}
    ENDSWITCH
ENDFUNCTION
```

##### Error handling
Error handling inside a parser is particularly difficult, if their an unexpected token anywhere, we cant parse the current expression, statement or declaration however their may be other errors further down. Most compilers will no halt at a single error, but continue so that any other errors can be reported in a single compilation which makes finding and fixing bugs much easier.

So whenever we call `expect()` we return the token we were expecting or `NULL` if the token was not what we expected. If the token was incorrect an error is produced and added to the array of errors. It is up to the caller how to handle and incorrect token, generally their is three different options.

* If the token is non fatal such as a `::` in a function definition, the parser can continue from were it is.
* If we are inside a statement and the token was fatal we can skip over the tokens until a `SEMI` token is reached, then the parser can continue safely.
* Otherwise the whole block must be skipped.

If the parser doesn't skip over the affected tokens, the same line of source code would have many conflicting errors which wouldn't make sense.

```markdown
FUNCTION skipBlock()
    WHILE token != LBRACE
        nextToken()
    ENDWHILE
    
    depth <- 0
    REPEAT
        IF token = LBRACE THEN
            depth <- depth + 1
        ELSEIF token = RBRACE THEN
            depth <- depth - 1
        ENDIF
        nextToken()
    WHILE depth > 0
ENDFUNCTION
```

```markdown
FUNCTION skipSemi()
    WHILE token != SEMI THEN
        nextToken()
    ENDWHILE
    nextToken()
ENDFUNCTION
```

#### parser.h
This is the header file for the parser implementation
```c
#pragma once

#include "uthash.h"
#include "all.h"
#include "queue.h"

#define ERROR_QUEUE_SIZE 10
#define MAX_ERRORS 10

typedef struct {
	char *name;
	Object *obj;
	UT_hash_handle hh;
} scope_object;

struct _scope {
	struct _scope *outer;
	scope_object *objects;
};
typedef struct _scope scope;

typedef struct {
	scope *scope;
    Token *tokens;
	ast_unit *ast;
	queue *error_queue;
} parser;

typedef enum {
	parser_error_expect_token,
	parser_error_expect_declaration,
	parser_error_expect_statement,
	parser_error_expect_expression,
	parser_error_expect_type,
	parser_error_expect_array_length,
	parser_error_expect_block,
	parser_error_expect_prefix,
	parser_error_expect_infix,
} parser_error_type;

typedef struct {
	parser_error_type type;
	Token *start;
	int length;

	union {
		struct {
			TokenType type;
		} expect_token;
	};
} parser_error;

// Parser interface
parser *new_parser(Token *tokens);
ast_unit *parse_file(parser *parser);

// Scope
scope *parser_new_scope(scope *outer);
void parser_enter_scope(parser *parser);
void parser_exit_scope(parser *parser);
bool parser_insert_scope(parser *parser, char *name, Object *object);
Object *parser_find_scope(parser *parser, char *name);

// Helpers
void parser_next(parser *parser);
Token *parser_expect(parser *parser, TokenType type);
void parser_expect_semi(parser *parser);
parser_error *new_error(parser *p, parser_error_type type, int length);
parser_error *new_error_token(parser *p, TokenType token_type);

// Declarations
Dcl *parse_declaration(parser *parser);
Dcl *parse_function_dcl(parser *parser);
Dcl *parse_variable_dcl(parser *parser);

// Statements
Smt *parse_statement(parser *parser);
Smt *parse_statement_from_string(char *src);
Smt *parse_block_smt(parser *p);
Smt *smtd(parser *p, Token *token);

// Expressions
Exp *parse_expression(parser *parser, int rbp);
Exp *parse_expression_from_string(char *src);
Exp *nud(parser *parser, Token *token);
Exp *led(parser *parser, Token *token, Exp *exp);
Exp *parse_key_value_exp(parser *parser);
Exp *parse_key_value_list_exp(parser *parser);
Exp *parse_array_exp(parser *parser);
Exp *parse_type(parser *parser);
Exp *parse_ident_exp_from_token(parser *parser, Token *token);
Exp *parse_ident_exp(parser *parser);
```

#### parser.c
This is the source file for the parser implementation
```c
#include "includes/parser.h"

// new_parser creates a new parser
parser *new_parser(Token *tokens) {
	parser *p = (parser *)malloc(sizeof(parser));
	p->tokens = tokens;
	p->scope = parser_new_scope(NULL);
	p->ast = new_ast_unit();
	p->error_queue = new_queue(sizeof(parser_error));
	return p;
}

// parse_file creates an abstract sytax tree from the tokens in parser 
ast_unit *parse_file(parser *p) {
	Dcl **dcls = malloc(0);
	int dclCount = 0;
	while(p->tokens->type != END) {
		Dcl *d = parse_declaration(p);
		dcls = realloc(dcls, ++dclCount * sizeof(Dcl *));
		memcpy(dcls + dclCount - 1, &d, sizeof(Dcl *));
	}

	ast_unit *f = malloc(sizeof(ast_unit));
	f->dcls = dcls;
	f->dclCount = dclCount;

	return f;
}

// creates a new empty scope
scope *parser_new_scope(scope *outer) {
	scope *inner_scope = (scope *)malloc(sizeof(scope));
	inner_scope->outer = outer;
	scope_object *objects = NULL;
	inner_scope->objects = objects;

	return inner_scope;
}

// parser_enter_scope enters a new inner scope
void parser_enter_scope(parser *p) {
	p->scope = parser_new_scope(p->scope);
}

// parser_exit_scope exits the current scope
void parser_exit_scope(parser *p) {
	// clear hash table and free all scope objects
	scope_object *obj, *tmp;
	HASH_ITER(hh, p->scope->objects, obj, tmp) {
		HASH_DEL(p->scope->objects, obj);
		free(obj);
	}

	// Move to outer scipe
	scope *outer = p->scope->outer;
	free(p->scope);
	p->scope = outer;
}

// parser_insert_scope inserts an object into the current scope
bool parser_insert_scope(parser *p, char *name, Object *object) {
	// check if name is already in scope
	scope_object *obj;
	HASH_FIND_STR(p->scope->objects, name, obj);
	if (obj != NULL) return false;

	// add object to scope
	obj = (scope_object *)malloc(sizeof(scope_object));
	obj->name = name;
	obj->obj = object;
	HASH_ADD_KEYPTR(hh, p->scope->objects, obj->name, strlen(obj->name), obj);
	return true;
}

// parser_find_scope finds an object in scope
Object *parser_find_scope(parser *p, char *name) {
	scope_object *obj;
	for (scope *scope = p->scope; scope != NULL; scope = scope->outer) {
		HASH_FIND_STR(scope->objects, name, obj);
		if (obj != NULL) return obj->obj;
	}

	return NULL;
}

// parser_next moves the parser onto the next token
void parser_next(parser *p) {
	p->tokens++;
}

// parser_expect checks that the current token is of type type, if true parser advances, 
// else an error message is created.
Token *parser_expect(parser *p, TokenType type) {
	Token *token = p->tokens;
	if(token->type == type) {
		parser_next(p);
		return token;
	} else {
		new_error_token(p, type);
		return NULL;
	}
}

// parser_expect_semi expects a semicolon
void parser_expect_semi(parser *p) {
	if(p->tokens->type == SEMI || p->tokens->type == END) {
		parser_next(p);
	} else {
		new_error_token(p, SEMI);
	}
}

// new_error added a new error to the queue
parser_error *new_error(parser *p, parser_error_type type, int length) {
	parser_error *error = queue_push_back(p->error_queue);
	error->type = type;
	error->start = p->tokens;
	error->length = length;
	return error;
}

// new_error_token added a new token error to the queue
parser_error *new_error_token(parser *p, TokenType token_type) {
	parser_error *error = new_error(p, parser_error_expect_token, 1);
	error->expect_token.type = token_type;
	return error;
}

char *format_error(char *src, parser_error *error) {
	return "";
}

// parse_declaration parse a decleration node
Dcl *parse_declaration(parser *p) {
	switch(p->tokens->type) {
		case PROC:
			return parse_function_dcl(p);
		case VAR:
		case IDENT:
			return parse_variable_dcl(p);
		default: {
			// expected a top level declaration
			new_error(p, parser_error_expect_declaration, 1);
			return NULL;
		}
	}
}

Dcl *parse_declaration_from_string(char *src) {
	parser *p = new_parser(Lex(src));
	return parse_declaration(p);
}

void parser_skip_next_block(parser *p) {
		// Move to start of block
		while(p->tokens->type != LBRACE) p->tokens++;
		
		// Skip over block (and all sub blocks)
		int depth = 0;
		do {
			if(p->tokens->type == LBRACE) depth++;
			else if(p->tokens->type == RBRACE) depth--;
			p->tokens++;
		} while(depth > 0);

		if(p->tokens->type == SEMI) p->tokens++;
}

void parser_skip_to_semi(parser *p) {
	// Move past first semi
	while(p->tokens->type != SEMI && p->tokens->type != END) p->tokens++;
	if(p->tokens->type == SEMI) p->tokens++;
}

// parse_function_dcl parses a function decleration
Dcl *parse_function_dcl(parser *p) {
	// Parse proc
	Token *proc = parser_expect(p, PROC);
	if (proc == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}

	// Parse function name
	Token *ident = parser_expect(p, IDENT);
	if (ident == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}
	char *name = ident->value; // function name
	
	// Parse argument seperator
	parser_expect(p, DOUBLE_COLON);
	// missing double colon is not fatel so countinue

	// Parse arguments
	Dcl *args = (Dcl *)malloc(0);
	int argCount = 0;
	while(p->tokens->type != ARROW && p->tokens->type != LBRACE) {
		if (argCount > 0) parser_expect(p, COMMA);
		// missing comma not fatel

		args = realloc(args, sizeof(Dcl) * ++argCount);

		// Construct argument
		Exp *type = parse_type(p); // arg type
		if (type == NULL) {
			parser_skip_next_block(p);
			return NULL;
		}

		// arg name
		Token *name_token = parser_expect(p, IDENT); 
		if (name_token == NULL) {
			parser_skip_next_block(p);
			return NULL;
		}
		char *name = name_token->value; 

		// add argument to list
		Dcl *arg = new_argument_dcl(p->ast, type, name);
		void *dest = memcpy(args + argCount - 1, arg, sizeof(Dcl));
	}
	
	Token *arrow = parser_expect(p, ARROW);
	if (arrow == NULL) {
		// arrow fatel since we dont know the return type
		parser_skip_next_block(p);
		return NULL;
	}

	Exp *return_type = parse_type(p);
	if (return_type == NULL) {
		parser_skip_next_block(p);
		return NULL;
	}

	// insert arguments into scope
	for (int i = 0; i < argCount; i++) {
		// insert into scope
		Object *obj = (Object *)malloc(sizeof(Object));
		obj->name = args[i].argument.name;
		obj->node = args + i;
		obj->type = argObj;
		parser_insert_scope(p, obj->name, obj);
	}

	// insert function into scope
	Dcl* function = new_function_dcl(p->ast, name, args, argCount, return_type, NULL);
	Object *obj = (Object *)malloc(sizeof(Object));
	obj->name = name;
	obj->node = function;
	obj->type = funcObj;
	parser_insert_scope(p, name, obj);
	
	// parse body
	Smt *body = parse_block_smt(p);
	function->function.body = body;

	if(p->tokens->type == SEMI) p->tokens++;

	return function;
}

Dcl *parse_variable_dcl(parser *p) {
	char *name;
	Exp *type = NULL;
	Exp *value;

	if(p->tokens->type == VAR) {
		p->tokens++;
		
		// Type
		type = parse_type(p);
		if (type == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}

		// Name
		Token *name_token = parser_expect(p, IDENT);
		if(name_token == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
		name = name_token->value;

		// Assign
		parser_expect(p, ASSIGN);
		// non fatel

		// Value
		value = parse_expression(p, 0);
		if(value == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
	} else {
		// Name
		Token *name_token = parser_expect(p, IDENT);
		if(name_token == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
		name = name_token->value;
		
		// Define
		parser_expect(p, DEFINE);
		// non fatel
		
		// Value
		value = parse_expression(p, 0);
		if(value == NULL) {
			parser_skip_to_semi(p);
			return NULL;
		}
	}

	Dcl *dcl = new_varible_dcl(p->ast, name, type, value);

	Object *obj = (Object *)malloc(sizeof(Object));
	obj->name = name;
	obj->node = dcl;
	obj->type = varObj;
	parser_insert_scope(p, name, obj);

	return dcl;
}

// Parses the next statement by calling smtd on the first token else handle
// the declaration/assignment
Smt *parse_statement(parser *p) {
	Token *t = p->tokens;
	Smt *smt = smtd(p, t);
	if (smt != NULL) {
		return smt;
	}

	// Statement is an assignment/declaration, so treat it like an expression
	// and transform it.
	Exp *exp = parse_expression(p, 0);
	if(exp == NULL || exp->type != binaryExp) {
		// expected assigment/declation statement
		if(exp == NULL) queue_pop_back(p->error_queue); // remove expression error
		new_error(p, parser_error_expect_statement, 1);
		parser_skip_to_semi(p);
	} 
	
	Exp *left = exp->binary.left;
	Exp *right = exp->binary.right;
	Token op = exp->binary.op;

	switch(op.type) {
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN:
			smt = new_binary_assignment_smt(p->ast, left, op.type, right);
			break;
		case DEFINE:
			// definition name
			if(left->type != identExp) {
				new_error_token(p, IDENT);
				parser_skip_to_semi(p);
				return NULL;
			}
			char *name = left->ident.name;

			smt = new_declare_smt(p->ast, new_varible_dcl(p->ast, name, NULL, right));
	
			// Added declaration to scope
			Object *obj =(Object *)malloc(sizeof(Object));
			obj->name = name;
			obj->node = smt->declare;
			obj->type = varObj;
			parser_insert_scope(p, name, obj);
	
			break;
		default:
			// Expected an assignment operator
			new_error(p, parser_error_expect_statement, 1);
			parser_skip_to_semi(p);
	}

	// If statment is null, the next tokens dont start a valid statement
	if(smt == NULL) {
		new_error(p, parser_error_expect_statement, 1);
		parser_skip_to_semi(p);
	}

	// Release the converted expression back into the pool
	pool_release(p->ast->exp_pool, exp);
	
	return smt;
}

Smt *parse_statement_from_string(char *src) {
	parser *p = new_parser(Lex(src));
    return parse_statement(p);
}

Smt *parse_block_smt(parser *p) {
	parser_expect(p, LBRACE);
	parser_enter_scope(p);

	// build list of statements
	int smtCount = 0;
	Smt *smts = (Smt *)malloc(sizeof(Smt) * 1024);
	Smt *smtsPrt = smts;
	while(p->tokens->type != RBRACE) {
		smtCount++;
		memcpy(smtsPrt, parse_statement(p), sizeof(Smt));
		if(p->tokens->type != RBRACE) parser_expect_semi(p);
		smtsPrt++;
	}
	smts = realloc(smts, sizeof(Smt) * smtCount);

	parser_expect(p, RBRACE);
	parser_exit_scope(p);

	Smt *s = new_block_smt(p->ast, smts, smtCount);
	return s;
}

// smtd parser the current token in the context of the start of a statement
Smt *smtd(parser *p, Token *token) {
	switch(token->type) {
		// return statement
		case RETURN: {
			p->tokens++;
			Smt *s = new_ret_smt(p->ast, parse_expression(p, 0));
			return s; 
		}
		// block statement
		case LBRACE:
			return parse_block_smt(p);
		
		// if statement
		case IF: {
			p->tokens++;
			
			Exp *cond = parse_expression(p, 0);
			Smt *block = parse_block_smt(p);
			Smt *elses = NULL;

			// Check for elseif/else
			if (p->tokens->type == ELSE) {
				p->tokens++;
				if (p->tokens->type == IF) {
					// else if, so recursivly parse else chain
					elses = parse_statement(p);
				} else {
					// final else statment only has a body
					elses = new_if_smt(p->ast, NULL, parse_statement(p), NULL);
				}
			}

			return new_if_smt(p->ast, cond, block, elses);
		}
		// for loop
		case FOR: {
			p->tokens++;

			// parse index
			Dcl *index = parse_variable_dcl(p);
			parser_expect_semi(p);

			// parse condition
			Exp *cond = parse_expression(p, 0);
			parser_expect_semi(p);

			// parse increment
			Smt *inc = parse_statement(p);
			
			// parse body
			Smt *body = parse_block_smt(p);

			return new_for_smt(p->ast, index, cond, inc, body);
		}
		// varible declaration
		case VAR: {
			return new_declare_smt(p->ast, parse_variable_dcl(p));
		}
		// increment expression
		case IDENT: {
			Exp *ident = parse_ident_exp(p);

			Token one_token = {INT, 0, 0, "1"};
			Exp *one_literal = new_literal_exp(p->ast, one_token);

			switch(p->tokens->type) {
				case INC:
					p->tokens++;
					return new_binary_assignment_smt(p->ast, ident, ADD_ASSIGN, one_literal);
				case DEC:
					p->tokens++;
					return new_binary_assignment_smt(p->ast, ident, SUB_ASSIGN, one_literal);
				default:
					// expression is assigment or declaration so let caller handle it
					p->tokens--; // go back to ident
					return NULL;
			}
		}
		default:
			return NULL;
	}
	return NULL;
}

// Parses the next expression by binding tokens until the left binding power is 
// <= right binding power (rbp)
Exp *parse_expression(parser *p, int rbp) {
	Exp *left;
	Token *t = p->tokens;
	parser_next(p);
	left = nud(p, t);
	if(left == NULL) return NULL;
	while (rbp < get_binding_power(p->tokens->type)) {
		t = p->tokens;
		parser_next(p);
		left = led(p, t, left);
		if(left == NULL) return NULL;
	}
	return left;
}

Exp *parse_expression_from_string(char *src) {
	parser *p = new_parser(Lex(src));
	return parse_expression(p, 0); 
}

// nud parses the current token in a prefix context (at the start of an (sub)expression)
Exp *nud(parser *p, Token *token) {
	switch (token->type) {
		case IDENT:
			return parse_ident_exp_from_token(p, token);
		
		case INT:
		case FLOAT:
		case HEX:
		case OCTAL:
		case STRING:
			return new_literal_exp(p->ast, *token);

		case NOT:
		case SUB:
			return new_unary_exp(p->ast, *token, parse_expression(p, 60));

		case LBRACE:
			return parse_key_value_list_exp(p);

		case LBRACK:
			return parse_array_exp(p);

		default: {
			// Expected a prefix token
			new_error(p, parser_error_expect_prefix, 1);
			return NULL;
		}
	}

	return NULL;
}

// led parses the current token in a infix contect (between two nodes)
Exp *led(parser *p, Token *token, Exp *exp) {
	int bp = get_binding_power(token->type);
	
	switch (token->type) {
		// binary expression
		case ADD:
		case SUB:
		case MUL:
		case QUO:
		case REM:
		case EQL:
		case NEQ:
		case GTR:
		case LSS:
		case GEQ:
		case LEQ: {
			return new_binary_exp(p->ast, exp, *token, parse_expression(p, bp));
		}

		// selector expression
		case PERIOD: {
			return new_selector_exp(p->ast, exp, parse_expression(p, bp));
		}

		// index expression
		case LBRACK: {
			Exp *index = parse_expression(p, 0);
			parser_expect(p, RBRACK);
			
			return new_index_exp(p->ast, exp, index);
		}

		// array/struct expression
		case LBRACE: {
			printf("LBrace!");
		}

		// call expression
		case LPAREN: {
			int argCount = 0;
			Exp *args = (Exp *)malloc(0);
			if(p->tokens->type != RPAREN) {
				// arguments are not empty so parse arguments
				while(true) {
					argCount++;
					
					args = realloc(args, argCount * sizeof(Exp));
					Exp *arg = parse_expression(p, 0);
					memcpy(args + argCount - 1, arg, sizeof(Exp));
					
					if(p->tokens->type == RPAREN) break;
					parser_expect(p, COMMA);
				}
			}
			parser_expect(p, RPAREN);

			return new_call_exp(p->ast, exp, args, argCount);
		}

		// right associative binary expression or assignments
		// if the expression is an assigment, return a binary statement and let parse_statement 
		// transform it into a statement
		case LAND:
		case LOR:
		case ASSIGN:
		case ADD_ASSIGN:
		case SUB_ASSIGN:
		case MUL_ASSIGN:
		case REM_ASSIGN:
		case OR_ASSIGN:
		case SHL_ASSIGN: 
		case DEFINE: {
			return new_binary_exp(p->ast, exp, *token, parse_expression(p, bp - 1));	
		}
		default: {
			// expected an infix expression
			new_error(p, parser_error_expect_infix, 1);
			return NULL;
		}
	}

	return NULL;
}

// Parses key value expressions in the form "expression:expression" or "expression"
Exp *parse_key_value_exp(parser *p) {
	Exp *keyOrVal = parse_expression(p, 0);
	Exp *key = NULL;
	Exp *value = NULL;
	
	if (p->tokens->type == COLON) {
		// Key/value belongs to structure expression
		p->tokens++;
		key = keyOrVal;
		value = parse_expression(p, 0);
	} else {
		// Key/value belongs to array expression
		value = keyOrVal;
	}

	return new_key_value_exp(p->ast, key, value);
}

Exp *parse_key_value_list_exp(parser *p) {
	int keyCount = 0;
	Exp *values = malloc(0);

	while(p->tokens->type != RBRACE) {
		keyCount++;
		values = realloc(values, keyCount * sizeof(Exp));
		Exp *keyValue = parse_key_value_exp(p);
		memcpy(values + keyCount - 1, keyValue, sizeof(Exp));
		
		if(p->tokens->type != RBRACE) parser_expect(p, COMMA);
	}

	return new_key_value_list_exp(p->ast, values, keyCount);
}

Exp *parse_array_exp(parser *p) {
	int valueCount = 0;
	Exp *values = malloc(0);
	while(p->tokens->type != RBRACK) {
		values = realloc(values, (++valueCount) * sizeof(Exp));
		Exp *value = parse_expression(p, 0);
		memcpy(values + valueCount - 1, value, sizeof(Exp));
		if (p->tokens->type != RBRACK) parser_expect(p, COMMA);
	}

	parser_expect(p, RBRACK);

	return new_array_exp(p->ast, values, valueCount);
}

// parse_type parses a type, adds an error if no type was found
Exp *parse_type(parser *p) {
	Exp *ident = parse_ident_exp(p);
	if (ident == NULL) {
		queue_pop_back(p->error_queue); // discard ident error
		new_error(p, parser_error_expect_type, 1);
		return NULL;
	}

	if(p->tokens->type == LBRACK) {
		// Type is an array type
		p->tokens++;
		Exp *length = parse_expression(p, 0);
		if(length == NULL) {
			new_error(p, parser_error_expect_array_length, 1);
			return NULL;
		}

		parser_expect(p, RBRACK);
		return new_array_type_exp(p->ast, ident, length);
	}

	return ident;
}

Exp *parse_ident_exp_from_token(parser *p, Token *token) {
	if(token->type != IDENT){
		// Add error to queue
		new_error_token(p, IDENT);
		return NULL;
	}

	char *name = token->value;
	Exp *ident = new_ident_exp(p->ast, name);
	Object *obj = parser_find_scope(p, name);
	ident->ident.obj = obj;
	
	return ident;
}

Exp *parse_ident_exp(parser *p) {
	Exp *ident = parse_ident_exp_from_token(p, p->tokens);
	parser_next(p);
	return ident;
}

```

### IR Generation
Having converted the source code to tokens and the tokens to an abstract syntax tree we now can transform it into LLVM IR which LLVM will compile to machine code. LLVM IR is much lower level than Fur however the LLVM C API does a lot of the heavy lifting for by constructing the intermediary language in memory.

#### If statements
LLVM IR does not have any if statements only conditional branches. To branch we must break the code into separate blocks which end with a terminating statements (such as a branch, return etc). Since if statements can be chained it make sense for the transformation to be recursive hence we need a two functions, the recursive `compileIfBranch()` function and the `compileIf()` function to call into the recursive function.

`compileIf()` just sets up a `endBlock` which is the block which the program will continue executing from.
```markdown
FUNCTION compileIf(irgen, ifSmt)
    endBlock <- LLVMAppendBasicBlock(irgen.function, "endBlock")
    compileIfBranch(irgen, ifSmt, _, endBlock)
ENDFUNCTION
```

`compileIfBranch()` takes four arguments: a reference to irgen which holds some state about the ir generation, the if node, the block to branch from (will be `NULL` for root level if branches) and the block to continue execution from. 

The first step is to check for a condition, if the condition is `NULL` then this branch must be an else branch therefore we compile the block and exit. `CompileBlockAt()` compiles a block statement and returns an `LLVMBlockRef` to were the execution should continue. We use `SetBlock()` to set it as are current block and check if its not terminated, if its not then we insert a branch to the end block.
```markdown
condition <- ifSmt.condition
IF condition = _ THEN
    outBlock <- CompileBlockAt(irgen, ifSmt.body, block)
    
    SetBlock(irgen, outBlock)
    IF LLVMGetBasicBlockTerminator(outBlock) = _ THEN
        LLVMBuildBr(irgen.builder, endBlock)
        SetBlock(irgen, endBlock)
    ENDIF
    
    RETURN
ENDIF
```

From this point we know the if branch must be an if or an else if. The next step is to get the `parent` block, which is the block to branch from.
```markdown
parent <- _
IF block = _ THEN
    parent <- irgen.block
ELSE
    parent <- block
ENDIF
```

`block` now becomes the block if the condition is true
```markdown
block <- LLVMAppendBasicBlock(irgen.function, "if")
```

`falseBlock` will be either the next else/else if branch or the block to continue execution
```markdown
falseBlock <- _
IF ifSmt.else != _ THEN
    falseBlock <- LLVMAppendBasicBlock(irgen.function, "else")
ELSE
    falseBlock <- endBlock
ENDIF
```

Now we can compile the body of the current if/else if branch, if the execution is not terminated we also branch to `endBlock`
```markdown
outBlock <- CompileBlockAt(irgen, ifSmt.body, block)
IF LLVMGetBasicBlockTerminator(outBlock) = _ THEN
    SetBlock(irgen, outBlock)
    LLVMBuildBr(irgen.builder, endBlock)
    SetBlock(irgen, parent)
ENDIF
```

The conditional branch instruction is next.
```markdown
condition <- CompileExp(irgen, cond);
LLVMBuildCondBr(irgen.builder, condition, block, falseBlock);
```

Next we check for any chaining else if/else blocks. If their are we recursively call this function to compile the whole chain.
```markdown
SetBlock(irgen, falseBlock)
IF ifSmt.else != _ THEN
    CompileIfBranch(irgen, ifSmt.else, falseBlock, endBlock)
ENDIF
```

Finally we set the end block as the current block so any further instructions execute from their.
```markdown
SetBlock(irgen, endBlock)
```

#### irgen.h
This is the header file for the IR generation implementation
```c
#pragma once

#include "all.h"

#include <llvm-c/Core.h>

struct _Irgen {
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMValueRef function;
    LLVMBasicBlockRef block;
};

typedef struct _Irgen Irgen;

Irgen *NewIrgen();

LLVMValueRef CompileExp(Irgen *irgen, Exp *e);
LLVMTypeRef CompileType(Exp *e);
LLVMValueRef CompileLiteralExp(Irgen *irgen, Exp *e);

void CompileDcl(Irgen *irgen, Dcl *d);
LLVMValueRef CompileFunction(Irgen *i, Dcl *d);

void CompileSmt(Irgen *i, Smt *s);
void CompileBlock(Irgen *i, Smt *s);

LLVMValueRef Cast(Irgen *irgen, LLVMValueRef value, LLVMTypeRef type);
```

#### irgen.c
This is the source file for the IR generation implementation
```c
#include "includes/irgen.h"

Irgen *NewIrgen() {
	Irgen *irgen = malloc(sizeof(Irgen));
    irgen->module = LLVMModuleCreateWithName("module");

    return irgen;
}

LLVMTypeRef CompileType(Exp *e) {
    switch(e->type) {
        case identExp:
            if (strcmp(e->ident.name, "int") == 0) return LLVMInt64Type();
            if (strcmp(e->ident.name, "i64") == 0) return LLVMInt64Type();
            if (strcmp(e->ident.name, "i32") == 0) return LLVMInt32Type();
            if (strcmp(e->ident.name, "i16") == 0) return LLVMInt16Type();
            if (strcmp(e->ident.name, "i8") == 0) return LLVMInt8Type();

            if (strcmp(e->ident.name, "float") == 0) return LLVMFloatType();
            if (strcmp(e->ident.name, "f32") == 0) return LLVMFloatType();
            if (strcmp(e->ident.name, "f64") == 0) return LLVMDoubleType();
        case arrayTypeExp: {
            LLVMTypeRef elementType = CompileType(e->arrayType.type);
            int length = atoi(e->arrayType.length->literal.value);
            return LLVMArrayType(elementType, length);
        }
        default:
            ASSERT(false, "Expected a type");
    }
}

LLVMValueRef CompileFunction(Irgen *irgen, Dcl *d) {
    ASSERT(d->type == functionDcl, "Expected function declaration");
    
    // compile argument types
    int argCount = d->function.argCount;
    LLVMTypeRef *argTypes = malloc(argCount * sizeof(LLVMTypeRef));
    for (int i = 0; i < argCount; i++) {
        argTypes[i] = CompileType(d->function.args[i].argument.type);
    }

    // compile return type
    LLVMTypeRef returnType = CompileType(d->function.returnType);

    // make function type
    LLVMTypeRef functionType = LLVMFunctionType(returnType, argTypes, argCount, 0); 

    // add function to module
    irgen->function = LLVMAddFunction(
        irgen->module, 
        d->function.name,
        functionType);

    // add function to node
    d->llvmValue = irgen->function;

    // create entry block and builder
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(irgen->function, "entry");
    irgen->block = entry;
    irgen->builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(irgen->builder, entry);

    // allocate arguments in entry block
    for (int i = 0; i < argCount; i++) {
        // get argument node
        Dcl *argNode = d->function.args + i;
        char *argName = argNode->argument.name;

        // allocate space for argument
        LLVMValueRef argAlloc = LLVMBuildAlloca(
            irgen->builder, 
            argTypes[i], 
            argName);
        
        // store alloc in node
        argNode->llvmValue = argAlloc;

        // store argument in allocated space
        LLVMValueRef argValue = LLVMGetParam(irgen->function, i);
        LLVMBuildStore(irgen->builder, argValue, argAlloc);
    }

    CompileBlock(irgen, d->function.body);

    // remove last block if empty
    if (LLVMGetFirstInstruction(irgen->block) == NULL) {
        LLVMDeleteBasicBlock(irgen->block);
    }

    irgen->block = NULL;
    return irgen->function;
}

// Sets the current block to the given basic block.
void SetBlock(Irgen *irgen, LLVMBasicBlockRef block) {
    irgen->block = block;
    LLVMPositionBuilderAtEnd(irgen->builder, block);
}

// Compiles a block in the current basic block.
void CompileBlock(Irgen *irgen, Smt *s) {
    ASSERT(s->type == blockSmt, "Expected block statment");
    
    // Compile all statements in block
    for (int i = 0; i < s->block.count; i++) {
        CompileSmt(irgen, &s->block.smts[i]);
    }
}

// Compiles a block at the given basic block.
LLVMBasicBlockRef CompileBlockAt(Irgen *irgen, Smt *s, LLVMBasicBlockRef block) {
    assert(s->type == blockSmt);
    
    // Move to target block
    LLVMBasicBlockRef parent = irgen->block;
    SetBlock(irgen, block);
    
    CompileBlock(irgen, s);
    
    // Restore parent block
    LLVMBasicBlockRef outBlock = irgen->block;
    SetBlock(irgen, parent);

    return outBlock;
}

void CompileReturn(Irgen *irgen, Smt *s) {
    ASSERT(s->type == returnSmt, "Expected a return statement");

    LLVMTypeRef functionType = LLVMTypeOf(irgen->function);
    LLVMTypeRef returnType = LLVMGetReturnType(LLVMGetReturnType(functionType));

    // build return instruction
    LLVMBuildRet(
        irgen->builder, 
        Cast(irgen, CompileExp(irgen, s->ret.result), returnType));
}

// Gets the allocation for an expression
LLVMValueRef GetAlloc(Irgen *irgen, Exp *e) {
    switch(e->type) {
        case identExp: {
            ASSERT(e->ident.obj != NULL, "Identifier doesnt have object");
            
            Dcl *dcl = e->ident.obj->node;
            return dcl->llvmValue;
        }
        case indexExp: {
            // Get the allocation of the expression
            LLVMValueRef alloc = GetAlloc(irgen, e->index.exp);
            
            // Get the element at the index
            LLVMValueRef index = CompileExp(irgen, e->index.index);
            LLVMValueRef zero = LLVMConstInt(LLVMInt64Type(), 0, false);
            LLVMValueRef indices[] = { zero, index };
            
            return LLVMBuildGEP(irgen->builder, alloc, indices, 2, "tmp");
        }
        default:
            ASSERT(false, "Cannot get alloc on unknown expression");
    }
}

void CompileAssignment(Irgen *irgen, Smt *s) {
    ASSERT(s->type == assignmentSmt, "Expected an assignment statement");

    LLVMValueRef alloc = GetAlloc(irgen, s->assignment.left);
    LLVMValueRef exp = CompileExp(irgen, s->assignment.right);
    LLVMBuildStore(irgen->builder, exp, alloc);
}

void CompileIfBranch(Irgen *irgen, Smt *s, LLVMBasicBlockRef block, LLVMBasicBlockRef endBlock) {
    ASSERT(s->type == ifSmt, "Expected if statement");
    
    Exp *cond = s->ifs.cond;
    if (cond == NULL) {
        assert(block != NULL);
        
        // Compile else block and exit
        LLVMBasicBlockRef outBlock = CompileBlockAt(irgen, s->ifs.body, block);
        SetBlock(irgen, outBlock);
        if (LLVMGetBasicBlockTerminator(outBlock) == NULL) {
            // block is not terminated so continue execution from end block
            LLVMBuildBr(irgen->builder, endBlock);
            SetBlock(irgen, endBlock);
        }

        return;
    }

    // parent block is the block to branch from
    LLVMBasicBlockRef parent;
    if (block == NULL) {
        parent = irgen->block;
    } else {
        parent = block;
    }
    // block if condition is true
    block = LLVMAppendBasicBlock(irgen->function, "if");
    
    // falseBlock is either the next else/elseif block or block to conitue execution
    LLVMBasicBlockRef falseBlock;
    if (s->ifs.elses != NULL) {
        falseBlock = LLVMAppendBasicBlock(irgen->function, "else");
    } else {
        falseBlock = endBlock;
    }

    // compile if block
    LLVMBasicBlockRef outBlock = CompileBlockAt(irgen, s->ifs.body, block);
    if (LLVMGetBasicBlockTerminator(outBlock) == NULL) {
        // block is not terminated so continue execution from end block
        SetBlock(irgen, outBlock);
        LLVMBuildBr(irgen->builder, endBlock);
        SetBlock(irgen, parent);
    }
    
    // Add the conditional branch
    LLVMValueRef condition = CompileExp(irgen, cond);
    LLVMBuildCondBr(irgen->builder, condition, block, falseBlock);

    // if their is a chaining elseif/else set its parent to the falseBlock
    SetBlock(irgen, falseBlock);
    if(s->ifs.elses != NULL) {
        CompileIfBranch(irgen, s->ifs.elses, falseBlock, endBlock);
    }

    // continue execution from the endBlock
    SetBlock(irgen, endBlock);
}

void CompileIf(Irgen *irgen, Smt *s) {
    LLVMBasicBlockRef endBlock = LLVMAppendBasicBlock(irgen->function, "endBlock");
    CompileIfBranch(irgen, s, NULL, endBlock);
}

void CompileFor(Irgen *irgen, Smt *s) {
    ASSERT(s->type == forSmt, "Expected for statement");

    // Compile loop varible
    CompileDcl(irgen, s->fors.index);

    // compile for body
    LLVMBasicBlockRef block = LLVMAppendBasicBlock(irgen->function, "for");
    LLVMBasicBlockRef outBlock = CompileBlockAt(irgen, s->fors.body, block);

    // branch into for loop
    LLVMValueRef outerCond = CompileExp(irgen, s->fors.cond);
    LLVMBasicBlockRef continueBlock = LLVMAppendBasicBlock(irgen->function, "endfor");
    LLVMBuildCondBr(irgen->builder, outerCond, block, continueBlock);

    // branch to loop or exit
    SetBlock(irgen, outBlock);
    CompileSmt(irgen, s->fors.inc);
    LLVMValueRef innerCond = CompileExp(irgen, s->fors.cond);
    LLVMBuildCondBr(irgen->builder, innerCond, block, continueBlock);

    // continue from continueBlock
    SetBlock(irgen, continueBlock);
}

void CompileSmt(Irgen *irgen, Smt *s) {
    switch (s->type) {
        case blockSmt:
            CompileBlock(irgen, s);
            break;
        
        case returnSmt:
            CompileReturn(irgen, s);
            break;
        
        case assignmentSmt:
            CompileAssignment(irgen, s);
            break;
        
        case declareSmt:
            CompileDcl(irgen, s->declare);
            break;
        
        case ifSmt:
            CompileIf(irgen, s);
            break;

        case forSmt:
            CompileFor(irgen, s);
            break;
        
        default:
            ASSERT(false, "TODO");
    }
}



void CompileVarible(Irgen *irgen, Dcl *d) {
    // get argument node
    char *varName = d->varible.name;

    // compile expression
    LLVMValueRef exp = CompileExp(irgen, d->varible.value);

    // get the type of the varible declaration
    LLVMTypeRef varType;
    if (d->varible.type != NULL) {
        varType = CompileType(d->varible.type);
        exp = Cast(irgen, exp, varType);
    } else {
        varType = LLVMTypeOf(exp);
    }

    LLVMValueRef varAlloc;
    if (d->varible.value->type == arrayExp) {
        varAlloc = exp;
    } else {
        // allocate space for varible
        varAlloc = LLVMBuildAlloca(
            irgen->builder, 
            varType, 
            varName);
            
        // store argument in allocated space
        LLVMBuildStore(irgen->builder, exp, varAlloc);
    }
        
    // store alloc in node
    d->llvmValue = varAlloc;

}

void CompileDcl(Irgen *irgen, Dcl *d) {
    switch(d->type) {
        case functionDcl:
            CompileFunction(irgen, d);
        case argumentDcl:
            ASSERT(false, "Cannot compile argument outside function declaration");
        case varibleDcl:
            CompileVarible(irgen, d);
    }
}

LLVMValueRef Cast(Irgen *irgen, LLVMValueRef value, LLVMTypeRef type) {
    LLVMTypeRef valueType = LLVMTypeOf(value);

    if(LLVMTypeOf(value) == type) return value;

    // create name base on value name + "_cast"
    char *valueName = (char *)LLVMGetValueName(value);
    char castName[(strlen(valueName) + 5) * sizeof(char)];
    strcpy(castName, valueName);
    strcpy(castName, "_cast");

    switch (LLVMGetTypeKind(valueType)) {
        // float type    
        case LLVMFloatTypeKind:
        case LLVMDoubleTypeKind:
            switch(LLVMGetTypeKind(type)) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFPCast(irgen->builder, value, type, castName);
                case LLVMIntegerTypeKind:
                    return LLVMBuildFPToSI(irgen->builder, value, type, castName);
                default:
                    ASSERT(false, "Casting float to non float/int type");
            }

        // integer type
        case LLVMIntegerTypeKind:
            switch(LLVMGetTypeKind(type)) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildSIToFP(irgen->builder, value, type, castName);
                case LLVMIntegerTypeKind:
                    return LLVMBuildIntCast(irgen->builder, value, type, castName);
                default:
                    ASSERT(false, "Casting integer to non float/int type");
            }

        default:
            ASSERT(false, "Cannot cast unknown LLVM type");
    }
}

LLVMValueRef CompileLiteralExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == literalExp, "Expected literal expression");
    
    switch (e->literal.type) {
        case INT:
            return LLVMConstIntOfString(LLVMInt64Type(), e->literal.value, 10);
        case FLOAT:
            return LLVMConstRealOfString(LLVMFloatType(), e->literal.value);
        case HEX:
            return LLVMConstIntOfString(LLVMInt64Type(), e->literal.value, 16);
        case OCTAL:
            return LLVMConstIntOfString(LLVMInt64Type(), e->literal.value, 8);
        case STRING:
            ASSERT(false, "Strings not implemented yet");
        default:
            ASSERT(false, "Unexpected literal type");
    }
}

LLVMValueRef CompileBinaryExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == binaryExp, "Expected binary expression");

    LLVMValueRef left = CompileExp(irgen, e->binary.left);
    LLVMValueRef right = CompileExp(irgen, e->binary.right);

    LLVMTypeRef leftType = LLVMTypeOf(left);
    LLVMTypeRef rightType = LLVMTypeOf(right);
    LLVMTypeRef nodeType;    

    // Check for unequal types
    if (leftType != rightType) {
        LLVMTypeKind leftKind = LLVMGetTypeKind(leftType);
        LLVMTypeKind rightKind = LLVMGetTypeKind(rightType);
        
        if(leftKind == LLVMIntegerTypeKind && rightKind == LLVMIntegerTypeKind) {
            nodeType = LLVMInt64Type();
            left = Cast(irgen, left, nodeType);
            right = Cast(irgen, right, nodeType);
        } else {
            // one or more sides are float so premote both sides to float
            nodeType = LLVMDoubleType();
            left = Cast(irgen, left, nodeType);
            right = Cast(irgen, right, nodeType);
        }
    } else {
        nodeType = leftType;
    }

    LLVMTypeKind nodeTypeKind = LLVMGetTypeKind(nodeType);

    // build name
    char *leftName = (char *)LLVMGetValueName(left);
    char *rightName = (char *)LLVMGetValueName(right);
    char name[strlen(leftName) + 1 + strlen(rightName)];
    strcpy(name, leftName);
    strcpy(name, TokenName(e->binary.op.type));
    strcpy(name, rightName);

    switch (e->binary.op.type) {
        case ADD:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFAdd(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildAdd(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot add non float/int type");
            }
        case SUB:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFSub(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildSub(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot sub non float/int type");
            }
        case MUL:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFMul(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildMul(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot multiply non float/int type");
            }
        case QUO:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFDiv(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildSDiv(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot divide non float/int type");
            }
        case REM:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFRem(irgen->builder, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildSRem(irgen->builder, left, right, name);
                default:
                    ASSERT(false, "Cannot mod non float/int type");
            }
        case LSS:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealULT, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSLT, left, right, name);
                default:
                    ASSERT(false, "Cannot less than non float/int type");
            }

        case LEQ:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealULE, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSLE, left, right, name);
                default:
                    ASSERT(false, "Cannot less than or equal non float/int type");
            }

        case GTR:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUGT, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSGT, left, right, name);
                default:
                    ASSERT(false, "Cannot greater than non float/int type");
            }
        case GEQ:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUGE, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntSGE, left, right, name);
                default:
                    ASSERT(false, "Cannot greater than or equal non float/int type");
            }

        case EQL:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUEQ, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntEQ, left, right, name);
                default:
                    ASSERT(false, "Cannot equal non float/int type");
            }

        case NEQ:
            switch(nodeTypeKind) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind:
                    return LLVMBuildFCmp(irgen->builder, LLVMRealUNE, left, right, name);
                case LLVMIntegerTypeKind:
                    return LLVMBuildICmp(irgen->builder, LLVMIntNE, left, right, name);
                default:
                    ASSERT(false, "Cannot not equal non float/int type");
            }

        case LAND:
        case LOR:
        case XOR:
        case SHL:
        case SHR:
        case AND_NOT:
        case AND:
        case OR:
            ASSERT(false, "TODO");
            break;
        default:
            ASSERT(false, "Unknown binary operator");
            break;
    }
}

LLVMValueRef CompileIdentExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == identExp, "Expected identifier expression");

    char *ident = e->ident.name;
    if(strcmp(ident, "true") == 0) return LLVMConstInt(LLVMInt1Type(), 1, false);
    if(strcmp(ident, "false") == 0) return LLVMConstInt(LLVMInt1Type(), 0, false);

    LLVMValueRef alloc = GetAlloc(irgen, e);
    return LLVMBuildLoad(irgen->builder, alloc, e->ident.name);
}

LLVMValueRef CompileUnaryExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == unaryExp, "Expected unary expression");

    LLVMValueRef exp = CompileExp(irgen, e->unary.right);
    switch(e->unary.op.type) {
        case ADD:
            return exp;
        case SUB: {
            // build name
            char *expName = (char *)LLVMGetValueName(exp);
            char *name = alloca(sizeof(char) * (strlen(expName) + 1));
            strcpy(name, "-");
            strcpy(name, expName);

            // insert "-1 * exp" instruction 
            LLVMTypeRef expType = LLVMTypeOf(exp);
            switch(LLVMGetTypeKind(expType)) {
                case LLVMFloatTypeKind:
                case LLVMDoubleTypeKind: {
                    LLVMValueRef minusOne = LLVMConstReal(expType, -1);
                    return LLVMBuildFMul(irgen->builder, minusOne, exp, name);
                }
                case LLVMIntegerTypeKind: {
                    LLVMValueRef minusOne = LLVMConstInt(expType, -1, true);
                    return LLVMBuildMul(irgen->builder, minusOne, exp, name);
                }
                default:
                    ASSERT(false, "Cannot multiply non float/int type by -1");
            } 
        }
        default:
            ASSERT(false, "Unkown unary operator");
    }
}

LLVMValueRef CompileCallExp(Irgen *irgen, Exp *e) {
    ASSERT(e->type == callExp, "Expected call expression");
    
    LLVMValueRef function = GetAlloc(irgen, e->call.function);

    // compile arguments
    int argCount = e->call.argCount;
    LLVMValueRef *args = malloc(argCount * sizeof(LLVMValueRef));
    for(int i = 0; i < argCount; i++) {
        args[i] = CompileExp(irgen, e->call.args + i);
    }

    return LLVMBuildCall(irgen->builder, function, args, argCount, "tmp");
} 

LLVMValueRef CompileArrayExp(Irgen *irgen, Exp *e) {
    assert(e->type == arrayExp);

    int valueCount = e->array.valueCount;
    bool isFloat = false;
    LLVMValueRef *values = alloca(valueCount * sizeof(LLVMValueRef));
    for (int i = 0; i < valueCount; i++) {
        values[i] = CompileExp(irgen, e->array.values + i);
        if (LLVMGetTypeKind(LLVMTypeOf(values[i])) == LLVMFloatTypeKind) {
            isFloat = true;
        }
    }

    LLVMTypeRef castType = isFloat ? LLVMFloatType() : LLVMInt64Type();
    LLVMTypeRef arrayType = LLVMArrayType(castType, valueCount);

    LLVMValueRef arrayAlloc = LLVMBuildAlloca(
        irgen->builder, 
        arrayType,
        "tmp");

    for (int i = 0; i < valueCount; i++) {
        values[i] = Cast(irgen, values[i], castType);
        
        LLVMValueRef indices[2] = { 
            LLVMConstInt(LLVMInt64Type(), 0, false),
            LLVMConstInt(LLVMInt64Type(), i, false), 
        };
        LLVMValueRef indexAlloc = LLVMBuildGEP(
            irgen->builder,
            arrayAlloc,
            indices,
            sizeof(indices) / sizeof(LLVMValueRef),
            "tmp");

        LLVMBuildStore(
            irgen->builder,
            values[i],
            indexAlloc);
    }

    return arrayAlloc;
}

LLVMValueRef CompileIndexExp(Irgen *irgen, Exp *e) {
    assert(e->type == indexExp);

    LLVMValueRef alloc = GetAlloc(irgen, e);
    return LLVMBuildLoad(irgen->builder, alloc, "tmp");
}

LLVMValueRef CompileExp(Irgen *irgen, Exp *e) {
    switch(e->type) {
        case literalExp:
            return CompileLiteralExp(irgen, e);
        case unaryExp:
            return CompileUnaryExp(irgen, e);
        case binaryExp:
            return CompileBinaryExp(irgen, e);
        case identExp:
            return CompileIdentExp(irgen, e);
        case callExp:
            return CompileCallExp(irgen, e); 
        case indexExp:
            return CompileIndexExp(irgen, e);
        case arrayExp:
            return CompileArrayExp(irgen, e);
        default:
            ASSERT(false, "Unknow expression type");
    }

    return NULL;
}
```