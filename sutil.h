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
#define TODO(msg) \
do { \
    fprintf(stderr, "%s:%d:%s: TODO: "msg"\n", __FILE__, __LINE__, __func__); \
    abort(); \
} while(0)

#define UNREACHABLE() \
do { \
    fprintf(stderr, "%s:%d:%s: reached unreachable code\n", __FILE__, __LINE__, __func__); \
    abort(); \
} while(0)

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

#define DEFINE_LIST(type, prefix) DEFINE_LIST_NAMED(type, prefix##_list)

#define DEFINE_LIST_NAMED(type, name) \
typedef struct { \
    size_t capacity; \
    size_t item_count; \
    type *items; \
} name;

#define list_new_named_ic(type, name, initial_capacity) (name){.capacity = initial_capacity, .item_count = 0, .items = (type*)malloc(sizeof(type)*initial_capacity)}
#define list_new_named(type, name) list_new_named_ic(type, name, LIST_DEFAULT_INITIAL_CAPACITY)

#define list_new_ic(type, prefix, initial_capacity) list_new_named_ic(type, prefix##_list, initial_capacity)
#define list_new(type, prefix) list_new_named_ic(type, prefix##_list, LIST_DEFAULT_INITIAL_CAPACITY)

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
#define list_size_cap(list) (list)->capacity * sizeof(*((list)->items))
#define list_size_count(list) (list)->item_count * sizeof(*((list)->items))

// #define list_foreach(list, name) for(typeof((list)->items) (name) = (list)->items; (name) < (list)->items + (list)->item_count; (name)++)
#define list_foreach(list, type, name) for(type *(name) = (list)->items; (name) < (list)->items + (list)->item_count; (name)++)

#define list_map(list, type, function, ...) \
do { \
    list_foreach(list, type, _sutil_item) { \
        function(_sutil_item, ##__VA_ARGS__); \
    } \
} while(0)

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

#define list_push_array(list, array) \
do { \
    for(size_t i = 0; i < sizeof(array)/sizeof(array[0]); i++) { \
        list_push(list, array[i]); \
    } \
} while(0)

#define list_get(list, n) (list)->items[n]
#define list_pop(list) (list)->item_count--
#define list_pop_n(list, n) (list)->item_count-=n
#define list_end(list) (list)->items[(list)->item_count-1]

#define list_get_s(list, n, result) ((n < (list)->item_count) ? ((result) = (list)->items[n], false) : true)
#define list_pop_s(list) (((list)->item_count > 0) ? ((list)->item_count--, false) : true)
#define list_pop_n_s(list, n) (((list)->item_count > n-1) ? ((list)->item_count-=n, false) : true)
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

MemArena arena_new_bs(size_t block_size);
void *arena_alloc(MemArena *arena, size_t size);
void arena_free(MemArena *arena);

#define arena_alloc_type(arena, type) (type*)arena_alloc(arena, sizeof(type))
#define arena_alloc_array(arena, type, size) (type*)arena_alloc(arena, sizeof(type)*size)

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
#define DS_FMT "%.*s"
#define DS_ARGS(ds) ((ds)->len > INT_MAX) ? INT_MAX : (int)((ds)->len), (ds)->data

#define ds_foreach(ds, name) for(char *(name) = (ds)->data; (name) < (ds)->data + (ds)->len; (name)++)

#define ds_len(ds) (ds)->len

#define ds_empty(ds) ((ds)->len == 0) ? 1 : 0

typedef struct {
    size_t len;
    char *data;
} DString;

DString ds_new(char *str);

bool ds_contains(DString *str, char needle);

char ds_peek_left(DString *str);
char ds_peek_right(DString *str);

void ds_trim_left(DString *str);
void ds_trim_right(DString *str);
void ds_trim(DString *str);

void ds_chop_left(DString *str);
void ds_chop_right(DString *str);

DString ds_chop_left_until(DString *str, char delimeter);
DString ds_chop_right_until(DString *str, char delimeter);

DString ds_chop_word_left(DString *str);
DString ds_chop_word_right(DString *str);

#ifdef SUTIL_IMPLEMENTATION

DString ds_new(char *str) {
    return (DString) {
        .len = strlen(str),
        .data = str
    };
};

bool ds_contains(DString *str, char needle) {
    ds_foreach(str, chr) {
        if(*chr == needle) return true;
    }
    return false;
}

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
SBuilder sb_new();
void sb_free(SBuilder *sb);

void sb_append(SBuilder *sb, char *str);
void sb_appendf(SBuilder *sb, char *format, ...);

char *sb_string(SBuilder *sb);

#define sb_print(sb) printf("%s\n", sb_string(sb))

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

#define SS(ss) ss.data

ShortString ss_new(char *str);
ShortString ss_new_f(char *format, ...);

#define ssprint(ss) printf("%s\n", SS(ss))
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
size_t file_readall_sz(char *path, char **out_ptr);
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
    return ds_new(*out_ptr);
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