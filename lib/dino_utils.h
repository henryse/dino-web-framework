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

#ifndef dino_dino_utils_h
#define dino_dino_utils_h

typedef struct stack_char_ptr_struct {
    char *data;
    size_t count;
    char **pointers;
} stack_char_ptr_t;

void *memory_clear(void *p, size_t n);

void *memory_alloc(size_t n);

void *memory_realloc(void *p, size_t n);

void memory_free(void *p);

void memory_cache_alloc(size_t n);

void memory_cache_clear();

void memory_cache_free();

void stack_ptr_free(stack_char_ptr_t *stack);

stack_char_ptr_t *stack_ptr_parse(stack_char_ptr_t *stack, const char *data, const char *delim);

char *trim_whitespace(char *str);

#define max(a, b) \
       ({ \
            __typeof__ (a) _a = (a); \
            __typeof__ (b) _b = (b); \
            _a > _b ? _a : _b; \
        })

#endif
