/**********************************************************************
//    Copyright (c) 2015 Henry Seurer
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without
//    restriction, including without limitation the rights to use,
//    copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following
//    conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//    OTHER DEALINGS IN THE SOFTWARE.
//
**********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "dino_utils.h"

typedef struct memory_cache_struct {
    size_t size;
    size_t freed;
    unsigned char *buffer;
} memory_cache_t;

typedef memory_cache_t *memory_cache_handle;

memory_cache_handle g_mem_cache = NULL;

stack_char_ptr_t *stack_ptr_alloc(const char *data) {
    stack_char_ptr_t *stack = (stack_char_ptr_t *) memory_alloc(sizeof(stack_char_ptr_t));

    stack->data = memory_alloc(strlen(data) + 1);
    memcpy(stack->data, data, strlen(data));

    return stack;
}

void stack_ptr_free(stack_char_ptr_t *stack) {
    if (NULL != stack) {
        if (NULL != stack->pointers) {
            memory_free(stack->pointers);
        }

        if (NULL != stack->data) {
            memory_free(stack->data);
        }

        memory_free(stack);
    }
}

stack_char_ptr_t *stack_ptr_push(stack_char_ptr_t *stack, char *ptr) {
    if (NULL != stack) {
        stack->count++;
        stack->pointers = (char **) memory_realloc(stack->pointers, stack->count * sizeof(char *));
        stack->pointers[stack->count - 1] = ptr;
    }

    return stack;
}

stack_char_ptr_t *stack_ptr_parse(stack_char_ptr_t *stack, const char *data, const char *delim) {
    if (NULL == stack) {
        stack = stack_ptr_alloc(data);
    }

    char *brkt = NULL;
    for (char *param = strtok_r(stack->data, delim, &brkt);
         param;
         param = strtok_r(NULL, delim, &brkt)) {
        stack = stack_ptr_push(stack, param);
    }

    return stack;
}

void *memory_clear(void *p, size_t n) {
    if (NULL != p) {
        bzero(p, n);
    }

    return p;
}

bool is_memory_cache_ptr(void *p) {
    if (NULL == g_mem_cache) {
        return false;
    }

    return (p >= (void *) g_mem_cache->buffer && p < (void *) (g_mem_cache->buffer + g_mem_cache->size));
}

void *memory_alloc(size_t n) {
    void *p = NULL;

    // See if it will fit in the cache:
    //
    if (NULL != g_mem_cache && n < g_mem_cache->freed) {
        p = g_mem_cache->buffer + (g_mem_cache->size - g_mem_cache->freed);
        g_mem_cache->freed -= n;
    }
    else {
        // If not allocate from memory.
        //
        p = malloc(n);
    }

    return memory_clear(p, n);
}

void *memory_realloc(void *p, size_t n) {
    if (NULL == p) {
        return memory_alloc(n);
    }

    if (is_memory_cache_ptr(p)) {
        if (n < g_mem_cache->freed) {
            // It wil still fit in the cache...
            //
            void *new_p = g_mem_cache->buffer + (g_mem_cache->size - g_mem_cache->freed);
            g_mem_cache->freed -= n;
            memcpy(new_p, p, n);
            return new_p;
        }
    }

    return realloc(p, n);
}

void memory_free(void *p) {
    if (NULL != p && !is_memory_cache_ptr(p)) {
        free(p);
    }
}

void memory_cache_alloc(size_t n) {
    if (NULL == g_mem_cache) {
        g_mem_cache = malloc(sizeof(memory_cache_t));
        memory_clear(g_mem_cache, sizeof(memory_cache_t));

        g_mem_cache->buffer = malloc(n);
        g_mem_cache->size = n;
        g_mem_cache->freed = n;
    }
}

void memory_cache_clear() {
    g_mem_cache->freed = g_mem_cache->size;
}

void memory_cache_free() {
    free(g_mem_cache->buffer);
    free(g_mem_cache);
}

char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    //
    while (isspace(*str)) {
        str++;
    }

    // All spaces?
    //
    if (*str == 0) {
        return str;
    }

    // Trim trailing space
    //
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    // Write new null terminator
    //
    *(end + 1) = 0;

    return str;
}