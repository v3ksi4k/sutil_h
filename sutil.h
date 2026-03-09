/*

MIT License

Copyright (c) 2026 v3ksi4k <vveksiakk@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef SUTIL_H
#define SUTIL_H

#include <stdlib.h>
#include <stdbool.h>

#define KB(n) (1000 * (n))
#define KiB(n) (1024 * (n))
#define MB(n) (1000000 * (n))
#define MiB(n) (1048576 * (n))
#define GB(n) (1000000000 * (n))
#define GiB(n) (1073741824 * (n))
#define MEM_ALIGN(n) (((n) + 7) & ~7)

#ifndef SUTIL_NO_LIST

#define DEFINE_LIST(type, prefix) \
typedef struct { \
    size_t capacity; \
    size_t item_count; \
    type *items; \
} prefix##_list;

#define list_new(list_type, capacity) (list_type){.capacity = capacity, .item_count = 0, .items = malloc(sizeof(type)*capacity)}

#define list_free(list) \
do { \
    (list)->capacity = 0; \
    (list)->item_count = 0; \
    if((list)->items != NULL) { \
        free((list)->items); \
        (list)->items = NULL; \
    } \
} while(0)

#define list_reset(list, capacity) \
do { \
    (list)->capacity = capacity; \
    (list)->item_count = 0; \
    if((list)->items != NULL) { \
        free((list)->items); \
        (list)->items = malloc((list)->capacity * sizeof(*((list->items)))); \
    } \
} while(0)

#define list_clear(list) (list)->item_count = 0

#define list_len(list) (list)->item_count
#define list_capacity(list) (list)->capacity

#define list_foreach(list, name) for(typeof((list)->items) (name) = (list)->items; (name) < (list)->items + (list)->item_count; (name)++)

#define list_push(list, item) \
do { \
    (list)->item_count++; \
    if((list)->capacity == 0) { \
        (list)->capacity = 1; \
        (list)->items = malloc((list)->capacity * sizeof(*((list)->items))); \
    } else if((list)->item_count > (list)->capacity) { \
        (list)->capacity = (list)->capacity * 2; \
        (list)->items = realloc((list)->items, (list)->capacity*sizeof(*((list)->items))); \
    } \
    (list)->items[(list)->item_count-1] = item; \
} while(0) \

#define list_get(list, n) (list)->items[n]
#define list_pop(list) (list)->item_count--
#define list_pop_n(list, n) (list)->item_count-=n
#define list_end(list) (list)->items[(list)->item_count-1]

#define list_get_s(list, n, result) ((n < (list)->item_count) ? ((result) = (list)->items[n], false) : true)
#define list_pop_s(list) (((list)->item_count > 0) ? ((list)->item_count--, false) : true)
#define list_pop_n_s(list, n) (((list)->item_count > n-1) ? ((list)->item_count-=n, false) : true)
#define list_end_s(list, result) (((list)->item_count > 0) ? ((result) = (list)->items[(list)->item_count-1], false) : true)

#endif // SUTIL_NO_LIST

#ifndef SUTIL_NO_ARENA

typedef struct {
    size_t capacity;
    size_t offset;
    void *items;
    MemBlock *next;
} MemBlock;

typedef struct {
    MemBlock *head; 
    MemBlock *tail; 
    size_t block_size;
} MemArena;

MemBlock *mem_block_new(size_t capacity);

MemArena arena_new(size_t block_size);
void *arena_alloc(MemArena *arena, size_t size);
void arena_free(MemArena *arena);

#endif // SUTIL_NO_ARENA

//TODO: Later change this to ifdef
#ifndef SUTIL_IMPLEMENTATION

#ifndef SUTIL_NO_ARENA

MemBlock *mem_block_new(size_t capacity) {
    MemBlock *result = (MemBlock*)malloc(sizeof(MemBlock));

    result->capacity = capacity;
    result->offset = 0;
    result->items = malloc(capacity);
    result->next = NULL;

    return result;
}

MemArena arena_new(size_t block_size) {
    MemArena result;    

    result.block_size = block_size;
    result.head = mem_block_new(block_size);
    result.tail = result.head;

    return result;
}

void *arena_alloc(MemArena *arena, size_t size) {
    size = MEM_ALIGN(size);
    if(size > arena->block_size) {
        MemBlock *new_block = mem_block_new(size);
        new_block->offset = size;
        arena->tail->next = new_block;
        arena->tail = new_block;
        return new_block->items;
    } else if(arena->tail->capacity < arena->tail->offset + size) {
        MemBlock *new_block = mem_block_new(arena->block_size);
        new_block->offset = size;
        arena->tail->next = new_block;
        arena->tail = new_block;
        return new_block->items;
    } else {
        void *result = (void*)((char*)arena->tail->items + arena->tail->offset);
        arena->tail->offset += size;
        return result;
    }
}

void arena_free(MemArena *arena) {
    MemBlock *block = arena->head;

    while(block != NULL) {
        MemBlock *tmp = block->next;

        free(block->items);
        free(block);

        block = tmp;
    }
}

#endif // SUTIL_NO_ARENA


#endif // SUTIL_IMPLEMENTATION

#endif // SUTIL_H