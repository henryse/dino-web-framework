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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dino_buffer.h"
#include "dino_utils.h"

typedef struct http_buffer_struct
{
    size_t allocated;
    size_t used;
    unsigned char *data;
}http_buffer;

BUFFER buffer_alloc(size_t size)
{
    http_buffer *buffer = alloc_and_clear_memory(sizeof(http_buffer));
    
    if (NULL != buffer)
    {
        buffer->allocated = size;
        buffer->used = size;
        buffer->data = alloc_and_clear_memory(size);
    }
    
    return buffer;
}


BUFFER buffer_realloc(BUFFER buffer_handle, size_t size)
{
    http_buffer *buffer = (http_buffer *)buffer_handle;
    if ( buffer == NULL )
    {
        return buffer_alloc(size);
    }
    
    if( buffer->data == NULL)
    {
        buffer->allocated = size;
        buffer->used = size;
        buffer->data = alloc_and_clear_memory(size);
    }
    
    size_t new_size = buffer->used + size;
    
    buffer->allocated = new_size;
    buffer->used = new_size;
    buffer->data = realloc(buffer->data, new_size);
    
    return buffer;
}


void buffer_free(BUFFER buffer_handle)
{
    if (NULL != buffer_handle)
    {
        http_buffer *buffer = (http_buffer *)buffer_handle;
        if (NULL != buffer->data)
        {
            buffer->allocated = 0;
            buffer->used = 0;
            free(buffer->data);
        }
        
        free(buffer);
    }
}

BUFFER buffer_append(BUFFER buffer_handle, const char *data, size_t size)
{
    http_buffer *buffer = NULL;
    char *ptr = NULL;
    
    if ( NULL == buffer_handle)
    {
        buffer = (http_buffer *)buffer_alloc(size);
        ptr = (char *)buffer->data;
    }
    else
    {
        buffer = (http_buffer *)buffer_handle;
        size_t offset = buffer->allocated;
        buffer = (http_buffer *)buffer_realloc(buffer_handle, buffer->allocated + size);
        ptr = (char *)buffer->data + offset;
    }
    
    if (NULL != buffer)
    {
        memcpy(ptr, data, size);
    }
    
    return buffer;
}

const char* buffer_data(BUFFER buffer_handle)
{
    if(buffer_handle != NULL)
    {
        http_buffer *buffer = (http_buffer *)buffer_handle;
        
        return (const char*)buffer->data;
    }
    
    return NULL;
}

size_t buffer_size(BUFFER buffer_handle)
{
    if(buffer_handle != NULL)
    {
        http_buffer *buffer = (http_buffer *)buffer_handle;
        
        return buffer->used;
    }

    return 0;
}