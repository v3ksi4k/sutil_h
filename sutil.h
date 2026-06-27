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

// To compile the hexdump utility use the following command:
// cc -DSUTIL_HEXDUMP -x c -o hexdump sutil.h

#ifndef SUTIL_HEXDUMP
#ifndef SUTIL_H
#define SUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>


// ----------Utility macros----------

/**
 * @brief Abort and print a debug message which contains `msg` to stderr
 */
#define TODO(msg) \
do { \
    fprintf(stderr, "%s:%d:%s: TODO: "msg"\n", __FILE__, __LINE__, __func__); \
    abort(); \
} while(0)

/**
 * @brief Abort and print a debug message to stderr
 */
#define UNREACHABLE() \
do { \
    fprintf(stderr, "%s:%d:%s: reached unreachable code\n", __FILE__, __LINE__, __func__); \
    abort(); \
} while(0)

/**
 * @brief Explicitly state that `x` is unused and get rid of the compiler warning
 */
#define UNUSED(x) (void)x


// ----------Numeric types----------
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
 
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;


// ----------Units and memory----------
#define KB(n) (1000UL * (n))
#define KiB(n) ((n) << 10)
#define MB(n) (1000000UL * (n))
#define MiB(n) ((n) << 20)
#define GB(n) (1000000000UL * (n))
#define GiB(n) ((n) << 30)

#define IN_KB(n) ((n) / 1000.0))
#define IN_KiB(n) ((n) / 1024.0)
#define IN_MB(n) ((n) / 1000000.0)
#define IN_MiB(n) ((n) / 1048576.0)
#define IN_GB(n) ((n) / 1000000000.0)
#define IN_GiB(n) ((n) / 1073741824.0)

#define MEM_ALIGN(n) (((n) + 7) & ~7)


// ----------List (Dynamic Array)----------
#define LIST_DEFAULT_INITIAL_CAPACITY 16

/**
 * @brief Define a struct called `prefix`_list which holds the fields of a list which holds data of type `type`
 */
#define DEFINE_LIST(type, prefix) DEFINE_LIST_NAMED(type, prefix##_list)

/**
 * @brief Define a struct called `name` which holds the fields of a list which holds data of type `type`
 */
#define DEFINE_LIST_NAMED(type, name) \
typedef struct { \
    size_t capacity; \
    size_t item_count; \
    type *items; \
} name;

/**
 * @brief Create a new list of type `name` which holds items of type `type` with an explicit initial capacity `initial_capacity`
 * @note The resulting list must be freed using `list_free`
 */
#define list_new_named_ic(type, name, initial_capacity) (name){.capacity = initial_capacity, .item_count = 0, .items = (type*)malloc(sizeof(type)*initial_capacity)}

/**
 * @brief Create a new list of type `name` which holds items of type `type`
 * @note The resulting list must be freed using `list_free`
 */
#define list_new_named(type, name) list_new_named_ic(type, name, LIST_DEFAULT_INITIAL_CAPACITY)

/**
 * @brief Create a new list of type `prefix`_list which holds items of type `type` with an explicit initial capacity `initial_capacity`
 * @note The resulting list must be freed using `list_free`
 */
#define list_new_ic(type, prefix, initial_capacity) list_new_named_ic(type, prefix##_list, initial_capacity)

/**
 * @brief Create a new list of type `prefix`_list which holds items of type `type`
 * @note The resulting list must be freed using `list_free`
 */
#define list_new(type, prefix) list_new_named_ic(type, prefix##_list, LIST_DEFAULT_INITIAL_CAPACITY)

/**
 * @brief Free the list `list`
 */
#define list_free(list) \
do { \
    (list)->capacity = 0; \
    (list)->item_count = 0; \
    if((list)->items != NULL) { \
        free((list)->items); \
        (list)->items = NULL; \
    } \
} while(0)

/**
 * @brief Free `list` and reinitialize it as an empty list
 */
#define list_reset(list, capacity) \
do { \
    (list)->capacity = capacity; \
    (list)->item_count = 0; \
    if((list)->items != NULL) { \
        free((list)->items); \
        (list)->items = malloc((list)->capacity * sizeof(*((list->items)))); \
    } \
} while(0)

/**
 * @brief Set the item count of `list` to 0 without freeing its contents
 * @note This is useful if you want to reinitialize the list without changing its current capacity
 */
#define list_clear(list) (list)->item_count = 0

/**
 * @brief Return the amount of items which `list` holds
 */
#define list_len(list) (list)->item_count

/**
 * @brief Return the current capacity of `list`
 */
#define list_capacity(list) (list)->capacity

/**
 * @brief Return the amount of bytes which were used to allocate the space for items of `list`
 */
#define list_size_cap(list) (list)->capacity * sizeof(*((list)->items))

/**
 * @brief Iterate through all items held by `list` in a for loop which intitializes a variable `name` and sets it to the current item 
 * @param name The name of a variable which will be set to the current item by the for loop
 */
#define list_foreach(list, type, name) for(type *(name) = (list)->items; (name) < (list)->items + (list)->item_count; (name)++)

/**
 * @brief Call the function `function` for each item of `list`, pass the item as the first argument and `...` as the remaining arguments
 * @param function The function which will be called in a format `function(list_item, ...)`
 * @param ... Remaining arguments of the function `function`
 */
#define list_map(list, type, function, ...) \
do { \
    list_foreach(list, type, _sutil_item) { \
        function(_sutil_item, ##__VA_ARGS__); \
    } \
} while(0)

/**
 * @brief Push `item` to the top of `list`
 */
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
} while(0)

/**
 * @brief Push an array of items (`array`) to the top of `list`
 */
#define list_push_array(list, array) \
do { \
    for(size_t i = 0; i < sizeof(array)/sizeof(array[0]); i++) { \
        list_push(list, array[i]); \
    } \
} while(0)

/**
 * @brief Get the `n`th item held by `list`
 * @warning This function is unsafe. The safe version of this function is `list_get_s`
 */
#define list_get(list, n) (list)->items[n]

/**
 * @brief Pop an item from the top of `list`
 * @warning This function is unsafe. The safe version of this function is `list_pop_s`
 */
#define list_pop(list) (list)->item_count--

/**
 * @brief Pop `n` items from the top of `list`
 * @warning This function is unsafe. The safe version of this function is `list_pop_n_s`
 */
#define list_pop_n(list, n) (list)->item_count-=n

/**
 * @brief Return the last item held by `list`
 * @warning This function is unsafe. The safe version of this function is `list_end_s`
 */
#define list_end(list) (list)->items[(list)->item_count-1]

/**
 * @brief Set `result` to the `n`th item held by list. Return a boolean indicating whether the operation succeeded
 * @result `true` if the operation succeeded and `result` was set or `false` otherwise
 * @note This is a safe version of `list_get`
 */
#define list_get_s(list, n, result) ((n < (list)->item_count) ? ((result) = (list)->items[n], false) : true)

/**
 * @brief Pop an item from the top of `list`. Return a boolean indicating whether the operation succeeded
 * @result `false` if the `list` was already empty or `true` otherwise
 * @note This is a safe version of `list_pop`
 */
#define list_pop_s(list) (((list)->item_count > 0) ? ((list)->item_count--, false) : true)

/**
 * @brief Pop `n` items from the top of `list`. Return a boolean indicating whether the operation succeeded
 * @result `false` if the size of `list` was lower than `n` or `true` otherwise
 * @note This is a safe version of `list_pop_n`
 */
#define list_pop_n_s(list, n) (((list)->item_count > n-1) ? ((list)->item_count-=n, false) : true)

/**
 * @brief Set `result` to the last item held by list. Return a boolean indicating whether the operation succeeded
 * @result `true` if the operation succeeded and `result` was set or `false` otherwise
 * @note This is a safe version of `list_end`
 */
#define list_end_s(list, result) (((list)->item_count > 0) ? ((result) = (list)->items[(list)->item_count-1], false) : true)


// ----------Memory Arena----------
#define ARENA_DEFAULT_BLOCK_SIZE 1024

typedef struct MemBlock MemBlock;

struct MemBlock {
    size_t capacity;
    size_t offset;
    void *items;
    MemBlock *next;
};

typedef struct {
    MemBlock *head; 
    MemBlock *tail; 
    size_t block_size;
} MemArena;

MemBlock *mem_block_new(size_t capacity);

/**
 * @brief Create a new `MemArena` structure and explicitly set the block size to `block_size`
 */
MemArena arena_new_bs(size_t block_size);

/**
 * @brief Allocate `size` bytes in `arena` and return a pointer to it
 * @note The memory then can be freed using `arena_free`
 */
void *arena_alloc(MemArena *arena, size_t size);

/**
 * @brief Free all memory allocated in `arena` which was allocated using functions from the `arena_alloc` family
 */
void arena_free(MemArena *arena);

/**
 * @brief Allocate enough memory in `arena` to hold 1 element of `type` and return a pointer to it
 */
#define arena_alloc_type(arena, type) (type*)arena_alloc(arena, sizeof(type))

/**
 * @brief Allocate enough memory in `arena` to hold an array of `type` with a size of `size` and return a pointer to it
 */
#define arena_alloc_array(arena, type, size) (type*)arena_alloc(arena, sizeof(type)*size)

/**
 * @brief Create a new `MemArena` structure with a default block size
 */
#define arena_new() arena_new_bs(ARENA_DEFAULT_BLOCK_SIZE)

#ifdef SUTIL_IMPLEMENTATION

MemBlock *mem_block_new(size_t capacity) {
    MemBlock *result = (MemBlock*)malloc(sizeof(MemBlock));

    result->capacity = capacity;
    result->offset = 0;
    result->items = malloc(capacity);
    result->next = NULL;

    return result;
}

MemArena arena_new_bs(size_t block_size) {
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

    arena->head = NULL;
    arena->tail = NULL;
}

#endif // SUTIL_IMPLEMENTATION


// ----------Dynamic String----------

/**
 * @brief The format parameter which should be used when trying to print dynamic strings with functions from the `printf` family
 * @note Remember to also provide `DS_ARGS`
 */
#define DS_FMT "%.*s"

/**
 * @brief The argument parameter which should be used when trying to print dynamic strings with functions from the `printf` family
 * @note Remember to use `DS_FMT`
 */
#define DS_ARGS(ds) ((ds)->len > INT_MAX) ? INT_MAX : (int)((ds)->len), (ds)->data

/**
 * @brief Iterate through all characters of `ds` in a for loop which initializes a variable `name` and sets it to the current character
 * @param name The name of a variable which will be set to the current character by the for loop
 */
#define ds_foreach(ds, name) for(char *(name) = (ds)->data; (name) < (ds)->data + (ds)->len; (name)++)

/**
 * @brief Returns the length of `ds`
 */
#define ds_len(ds) (ds)->len

/**
 * @brief Returns the `char*` held by `ds`
 */
#define ds_str(ds) (ds)->data

/**
 * @brief Returns 1 if `ds` is empty or 0 if it isn't
 */
#define ds_empty(ds) ((ds)->len == 0) ? 1 : 0

typedef struct {
    size_t len;
    char *data;
    char *orig_ptr;
} DString;

/**
 * @brief Creates a new dynamic string with contents `str`
 * @param str Pointer to a null-terminated string
 * @note The result must be freed using `ds_free`
 */
DString ds_new(char *str);

/**
 * @brief Creates a new dynamic string containing a printf-formatted string
 * @param format Format string which will be passed to printf
 * @param ... Variadic arguments which will be passed to printf
 * @note The result must be freed using `ds_free`
 */
DString ds_new_fmt(char *format, ...);

/**
 * @brief Creates a new dynamic string without cloning the string pointed to by `str`
 * @param str Pointer to a null-terminated string
 * @warning Contrary to ds_new and ds_new_fmt you musn't call ds_free on the result
 */
DString ds_new_wrap(char *str);

/**
 * @brief Clone the string held by a dynamic string `str` and return a pointer to it
 * @result Pointer to a clone of a null-terminated string held by `str`
 * @note The result must be freed
 */
char *ds_clone_str(DString *str);

/**
 * @brief Clone the dynamic string `str` and return the clone
 * @note The returned `DString` must be freed using `ds_free`
 */
DString ds_clone_ds(DString *str);

/**
 * @brief Free the dynamic string `str`
 * @warning This function musn't be called on dynamic strings which are a result of `ds_new_wrap`
 */
void ds_free(DString *str);

/**
 * @brief Put a null-terminator at the end of a string held by `str`
 * @warning This will modify the original string passed into `ds_new`
 */
void ds_terminate(DString *str);

/**
 * @brief Checks whether the dynamic string `str` contains `needle`
 */
bool ds_contains(DString *str, char needle);

/**
 * @brief Checks whether the dynamic string `str` is equal to `needle`
 */
bool ds_strcmp(DString *str, char *needle);

/**
 * @brief Return a char from the left side of `str`, or the first char of `str` on error
 */
char ds_peek_left(DString *str);

/**
 * @brief Return a char from the right side of `str`, or the last char of `str` on error
 */
char ds_peek_right(DString *str);

/**
 * @brief Trim all whitespaces from the left side of `str`
 */
void ds_trim_left(DString *str);

/**
 * @brief Trim all whitespaces from the right side of `str`
 */
void ds_trim_right(DString *str);

/**
 * @brief Trim all whitespaces from both sides of `str`
 */
void ds_trim(DString *str);

/**
 * @brief Pop a char from the left side of `str`
 */
void ds_chop_left(DString *str);

/**
 * @brief Pop a char from the right side of `str`
 */
void ds_chop_right(DString *str);

/**
 * @brief Pop chars from the left side of `str` until `delimeter` is encountered. Create a child `DString` with the chopped contents
 * @result A `DString` pointing to the chopped part of `str`
 * @warning The resulting `DString` musn't be freed as it will be freed with its parent
 */
DString ds_chop_left_until(DString *str, char delimeter);

/**
 * @brief Pop chars from the right side of `str` until `delimeter` is encountered. Create a child `DString` with the chopped contents
 * @result A `DString` pointing to the chopped part of `str`
 * @warning The resulting `DString` musn't be freed as it will be freed with its parent
 */
DString ds_chop_right_until(DString *str, char delimeter);

/**
 * @brief Pop chars from the left side of `str` until a whitespace character is encountered. Create a child `DString` with the chopped contents
 * @result A `DString` pointing to the chopped part of `str`
 * @warning The resulting `DString` musn't be freed as it will be freed with its parent
 */
DString ds_chop_word_left(DString *str);

/**
 * @brief Pop chars from the right side of `str` until a whitespace character is encountered. Create a child `DString` with the chopped contents
 * @result A `DString` pointing to the chopped part of `str`
 * @warning The resulting `DString` musn't be freed as it will be freed with its parent
 */
DString ds_chop_word_right(DString *str);

#ifdef SUTIL_IMPLEMENTATION

DString ds_new(char *str) {
    size_t len = strlen(str);
    char *data = (char*)malloc(len + 1);
    strncpy(data, str, len + 1);

    return (DString) {
        .len = len,
        .data = data,
        .orig_ptr = data
    };
}

DString ds_new_fmt(char *format, ...) {
    va_list args;
    va_list args_tmp;

    va_copy(args_tmp, args);

    size_t len = vsnprintf(NULL, 0, format, args_tmp);
    char *data = (char*)malloc(len + 1);

    vsnprintf(data, len + 1, format, args);

    return (DString) {
        .len = len,
        .data = data,
        .orig_ptr = data
    };
}

DString ds_new_wrap(char *str) {
    return (DString) {
        .len = strlen(str),
        .data = str,
        .orig_ptr = str
    };
};

char *ds_clone_str(DString *str) {
    char *result;

    result = (char*)malloc(str->len + 1);
    memcpy(result, str->data, str->len);
    result[str->len] = '\0';

    return result;
};

DString ds_clone_ds(DString *str) {
    DString result;
    char *data;
    size_t len = str->len;

    data = (char*)malloc(str->len + 1);
    memcpy(data, str->data, str->len);
    data[str->len] = '\0';

    result = (DString){ .data = data, .orig_ptr = data, .len = len };

    return result;
};

void ds_free(DString *str) {
    free(str->orig_ptr);
    str->len = 0;
}

void ds_terminate(DString *str) {
    str->data[str->len] = '\0';
};

bool ds_contains(DString *str, char needle) {
    ds_foreach(str, chr) {
        if(*chr == needle) return true;
    }
    return false;
}

bool ds_strcmp(DString *str, char *needle) {
    return strncmp(str->data, needle, str->len);
};

char ds_peek_left(DString *str) {
    if(str->len == 0) return '\0';
    else return str->data[0];
}

char ds_peek_right(DString *str) {
    if(str->len == 0) return '\0';
    else return str->data[str->len-1];
}

void ds_trim_left(DString *str) {
    while(str->len > 0 && isspace(str->data[0])) {
        str->len--;
        str->data++;
    }
}

void ds_trim_right(DString *str) {
    while(str->len > 0 && isspace(str->data[str->len-1])) {
        str->len--;
    }
}

void ds_trim(DString *str) {
    ds_trim_right(str);
    ds_trim_left(str);
}

void ds_chop_left(DString *str) {
    if(str->len > 0) {
        str->len--;
        str->data++;
    }
}

void ds_chop_right(DString *str) {
    if(str->len > 0) str->len--;
}

DString ds_chop_left_until(DString *str, char delimeter) {
    DString res;
    res.data = str->data;

    size_t orig_len = str->len;
    while(str->len > 0 && str->data[0] != delimeter) {
        str->len--;
        str->data++;
    }
    str->len--;
    str->data++;

    res.len = orig_len - str->len;

    return res;
};

DString ds_chop_right_until(DString *str, char delimeter) {
    DString res;

    size_t orig_len = str->len;
    while(str->len > 0 && str->data[str->len - 1] != delimeter) {
        str->len--;
    }

    res.data = str->data + str->len;
    res.len = orig_len - str->len;

    return res;
};

DString ds_chop_word_left(DString *str) {
    DString res;

    while(str->len > 0 && isspace(str->data[0])) {
        str->len--;
        str->data++;
    }

    size_t orig_len = str->len;
    res.data = str->data;

    while(str->len > 0 && !isspace(str->data[0])) {
        str->len--;
        str->data++;
    }
    str->len--;
    str->data++;

    res.len = orig_len - str->len - 1;

    return res;
}

DString ds_chop_word_right(DString *str) {
    DString res;

    while(str->len > 0 && isspace(str->data[str->len - 1])) {
        str->len--;
    }

    size_t orig_len = str->len;
    while(str->len > 0 && !isspace(str->data[str->len - 1])) {
        str->len--;
    }

    res.data = str->data + str->len;
    res.len = orig_len - str->len;

    return res;
}

#endif // SUTIL_IMPLEMENTATION


// ----------String Builder----------

/**
 * @brief Constructs a null-terminated string out of strings contained by `sb` and returns a poniter to it
 * @result Pointer to a null-terminated string built using `sb`
 * @note The result must be freed
 * @note This is an alias of `sb_string`
 */
#define SB(sb) sb_string(sb)

typedef struct StringChunk StringChunk;

struct StringChunk {
    StringChunk *next;
    char *data;
    size_t size;
};

typedef struct {
    StringChunk *head;
    StringChunk *tail;
    size_t size;
} SBuilder;

StringChunk *string_chunk_new(char *data, size_t data_len);

/**
 * @brief Create a new string builder and return it
 * @result A newly created SBuilder instance
 * @note The result must be freed with `sb_free`
 */
SBuilder sb_new();

/**
 * @brief Free the contents of `sb`. This will free of the strings that it accumulated
 */
void sb_free(SBuilder *sb);

/**
 * @brief Append the string `str` to `sb` by cloning it
 */
void sb_append(SBuilder *sb, char *str);

/**
 * @brief Append a printf-formatted string to `sb`
 * @param format Format string which fill be passed to printf
 * @param ... Variadic arguments which will be passed to pritnf
 */
void sb_appendf(SBuilder *sb, char *format, ...);

/**
 * @brief Constructs a null-terminated string out of strings contained by `sb` and returns a poniter to it
 * @result Pointer to a null-terminated string built using `sb`
 * @note The result must be freed
 * @note This function has a shorter alias `SB`
 */
char *sb_string(SBuilder *sb);

/**
 * @result The total size of all strings accumulated in `sb` in bytes
 */
#define sb_size(sb) (sb)->size

#ifdef SUTIL_IMPLEMENTATION

StringChunk *string_chunk_new(char *data, size_t data_len) {
    StringChunk *result = (StringChunk*)malloc(sizeof(StringChunk) + data_len);

    result->data = (char*)(result + 1);
    memcpy(result->data, data, data_len);
    result->size = data_len;
    result->next = NULL;

    return result;
}

SBuilder sb_new() {
    SBuilder result;

    result.head = NULL;
    result.size = 0;
    result.tail = NULL;

    return result;
};

void sb_append(SBuilder *sb, char *str) {
    size_t str_len = strlen(str);
    StringChunk *chunk = string_chunk_new(str, str_len);

    if(sb->head == NULL) {
        sb->head = chunk; 
        sb->tail = chunk;
    } else {
        sb->tail->next = chunk;
        sb->tail = chunk;
    }

    sb->size += str_len;
};

void sb_appendf(SBuilder *sb, char *format, ...) {
    va_list args;
    va_list args_cp;

    va_start(args, format);
    va_copy(args_cp, args);

    char buffer[1024];
    char *buf = buffer;

    size_t size = vsnprintf(buf, sizeof(buffer), format, args);
    
    if(size > sizeof(buffer)) {
        buf = (char*)malloc(size);
        vsnprintf(buf, size, format, args);
    }

    va_end(args);

    StringChunk *chunk = string_chunk_new(buf, size);
    if(buf != buffer) free(buf);

    if(sb->head == NULL) {
        sb->head = chunk; 
        sb->tail = chunk;
    } else {
        sb->tail->next = chunk;
        sb->tail = chunk;
    }

    sb->size += size;
}

char *sb_string(SBuilder *sb) {
    size_t offset = 0;
    char *result = (char*)malloc(sb->size + 1);
    StringChunk *chunk = sb->head;

    while(chunk != NULL) {
        memcpy(result + offset, chunk->data, chunk->size);
        offset += chunk->size;
        chunk = chunk->next;
    }

    result[sb->size] = '\0';

    return result;
}

void sb_free(SBuilder *sb) {
    StringChunk *chunk = sb->head;

    while(chunk != NULL) {
        StringChunk *tmp = chunk->next;

        free(chunk);

        chunk = tmp;
    }

    sb->head = NULL;
    sb->tail = NULL;
    sb->size = 0;
}

#endif // SUTIL_IMPLEMENTATION


// ----------Short String----------
typedef struct {
    char data[64];
} ShortString;

/**
 * @brief Represent the short string structure as a null-terminated string
 * @result Pointer to a null-terminated string with the contents of `ss`
 * @note This function can be used to eaisly print short strings
 * @warning The result must NOT be freed
 */
#define SS(ss) ss.data

/**
 * @brief Initialize a new ShortString and set its contents to `str`
 * @param str A null-terminated string which will be truncated to 64 bytes (including the null-terminator)
 */
ShortString ss_new(char *str);

/**
 * @brief Intialize a new ShortString and set its contents to a printf-formatted string
 * @param format Format string which fill be passed to printf
 * @param ... Variadic arguments which will be passed to pritnf
 */
ShortString ss_new_f(char *format, ...);

/**
 * @brief Print `ss` into `stdout` with a trailing newline
 */
#define ssprint(ss) printf("%s\n", SS(ss))

/**
 * @brief Print `ss` into `file` with a trailing newline
 */
#define ssfprint(ss, file) fprintf(file, "%s\n", SS(ss))

#ifdef SUTIL_IMPLEMENTATION

ShortString ss_new(char *str) {
    ShortString result;

    memcpy(result.data, str, sizeof(result.data) - 1);
    result.data[sizeof(result.data)-1] = '\0';

    return result;
}

ShortString ss_new_f(char *format, ...) {
    ShortString result;
    va_list args;

    va_start(args, format);
    vsnprintf(result.data, sizeof(result.data), format, args);
    va_end(args);

    return result;
}

#endif // SUTIL_IMPLEMENTATION


// ----------File Utilities----------

/**
 * @brief Read an entire file from `path`, return its contents as a null-terminates string to `out_ptr` and their size
 * @param path A pre-validated path to a file 
 * @param out_ptr A pointer which will point to a null-terminated string containing the contents of the file
 * @result Size of the contents of the file in bytes
 * @note If the size of the contents is not needed, use `file_readall`
 */
size_t file_readall_sz(char *path, char **out_ptr);

/**
 * @brief Read an entire file from `path` and return its contents as a null-terminated string
 * @param path A pre-validated path to a file 
 * @result Pointer to a null-terminated string with the contents of the file
 * @note If the size of the contents is needed, use `file_readall`
 */
char *file_readall(char *path);

#ifdef SUTIL_IMPLEMENTATION

size_t file_readall_sz(char *path, char **out_ptr) {
    SBuilder sb = sb_new();
    FILE *f = fopen(path, "r");
    char buffer[1024];

    while(!feof(f)) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, f);
        buffer[bytes_read] = '\0';
        sb_append(&sb, buffer);
    }
    fclose(f);

    *out_ptr = sb_string(&sb);
    return sb_size(&sb);
}

char *file_readall(char *path) {
    char *result;
    
    file_readall_sz(path, &result);
    
    return result;
}

DString file_readall_ds(char *path, char **out_ptr) {
    *out_ptr = file_readall(path);
    return ds_new_wrap(*out_ptr);
}

#endif // SUTIL_IMPLEMENTATION


// ----------Timer----------
#define timer_set(name) clock_t _sutil_clock_##name = clock()
#define timer_elapsed(name) (double)(clock() - _sutil_clock_##name) / CLOCKS_PER_SEC
#define timer_print(name) printf("Elapsed (name): %fs\n", timer_elapsed(name))


#endif // SUTIL_H
#else

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

void usage() {
    fprintf(stderr, "Usage: hexdump <variable_name> [filename]\n");
    exit(1);
}

int main(int argc, char **argv) {
    FILE *f;
    bool use_stdin = false;

    if(argc == 2) {
        use_stdin = true;
        f = stdin;
    } else if(argc != 3) usage();
    
    struct stat ignore;

    if(!use_stdin && stat(argv[2], &ignore) == -1) usage();

    printf("unsigned char %s[] = {\n    ", argv[1]);

    if(!use_stdin) f = fopen(argv[2], "r");

    unsigned char buffer[1024];

    while(!feof(f)) {
        size_t size = fread(buffer, 1, sizeof(buffer) - 4, f);
        for(size_t i = 1; i < size + 1; i++) {
            printf("0x%02X,", buffer[i-1]);
            if(i % 10 == 0) printf("\n    ");
        }
    }

    if(!use_stdin) fclose(f);

    printf("\n};\n");
}

#endif // SUTIL_HEXDUMP