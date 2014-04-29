//    Copyright (c) 2014 Henry Seurer
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

#include <stdlib.h>
#include <strings.h>
#include "dino_utils.h"

void *clear_memory(void *p, size_t n)
{
    if (NULL != p)
    {
        bzero(p, n);
    }
    
    return p;
}

void *malloc_and_clear(size_t n)
{
    return clear_memory(malloc(n), n);
}

stack_char_ptr *stack_ptr_alloc()
{
    return malloc_and_clear(sizeof(stack_char_ptr));
}

void stack_ptr_free(stack_char_ptr *stack)
{
    if (NULL != stack)
    {
        if (stack->ptrs != NULL)
        {
            free(stack->ptrs);
        }
        free(stack);
    }
}

stack_char_ptr *stack_ptr_push(stack_char_ptr *stack, char *ptr)
{
    if (NULL == stack)
    {
        stack = stack_ptr_alloc();
    }
    
    if (NULL != stack)
    {
        stack->count++;
        stack->ptrs = (char **)realloc(stack->ptrs, stack->count * sizeof(char *));
        stack->ptrs[stack->count - 1] = ptr;
    }
    
    return stack;
}

stack_char_ptr *stack_ptr_parse(stack_char_ptr *stack, char *path, char *delim)
{
    if (NULL == stack)
    {
        stack = stack_ptr_alloc();
    }

    char *brkt = NULL;
    for (char *param = strtok_r(path, delim, &brkt);
         param;
         param = strtok_r(NULL, delim, &brkt))
    {
        stack = stack_ptr_push(stack, param);
    }

    return stack;
}
