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

#include <stdio.h>
#include <string.h>
#include "dino_buffer.h"
#include "dino_utils.h"

#define BUFFER_BLOCK_SIZE 256

typedef struct http_buffer_struct {
    size_t allocated;
    size_t used;
    unsigned char *data;
} http_buffer_t;

size_t calculate_allocated_size(size_t size) {
    // Figure out how many we can get...
    //
    size_t num_blocks = size / BUFFER_BLOCK_SIZE;

    // Was there a remainder?
    //
    if (size % BUFFER_BLOCK_SIZE) {
        // Bump it once more to pick up the remainder
        //
        ++num_blocks;
    }

    // Calculate the number of bytes to allocate
    //
    return num_blocks * BUFFER_BLOCK_SIZE;
}

void buffer_initialize(BUFFER buffer_handle, size_t size) {
    if (NULL != buffer_handle) {
        http_buffer_t *buffer = (http_buffer_t *) buffer_handle;

        if (buffer->data) {
            memory_free(buffer->data);
        }

        buffer->allocated = calculate_allocated_size(size);
        buffer->data = memory_alloc(buffer->allocated);
        buffer->used = size;
    }
}

BUFFER buffer_alloc_initialize(size_t size) {
    http_buffer_t *buffer = memory_alloc(sizeof(http_buffer_t));

    buffer_initialize(buffer, size);

    return buffer;
}

BUFFER buffer_realloc(BUFFER buffer_handle, size_t size) {
    if (NULL == buffer_handle) {
        return buffer_alloc_initialize(size);
    }

    http_buffer_t *buffer = (http_buffer_t *) buffer_handle;

    if (NULL == buffer->data) {
        buffer_initialize(buffer_handle, size);
    }
    else {
        if (buffer->allocated < size) {
            buffer->allocated = calculate_allocated_size(size);
            buffer->data = memory_realloc(buffer->data, buffer->allocated);
        }

        buffer->used = size;
    }

    return buffer;
}

void dino_buffer_free(BUFFER buffer_handle) {
    if (NULL != buffer_handle) {
        http_buffer_t *buffer = (http_buffer_t *) buffer_handle;
        if (NULL != buffer->data) {
            buffer->allocated = 0;
            buffer->used = 0;
            memory_free(buffer->data);
        }

        memory_free(buffer);
    }
}

BUFFER dino_buffer_append_data(BUFFER buffer_handle, const char *data, size_t size) {
    http_buffer_t *buffer = (http_buffer_t *) buffer_handle;

    char *ptr = NULL;
    size_t used = 0;

    if (NULL == buffer) {
        buffer = (http_buffer_t *) buffer_alloc_initialize(size);
    }
    else {
        used = buffer->used;
        buffer = (http_buffer_t *) buffer_realloc(buffer_handle, buffer->used + size);
    }

    if (NULL != buffer) {
        ptr = (char *) buffer->data + used;
        memcpy(ptr, data, size);
    }

    return buffer;
}

BUFFER dino_buffer_append_char(BUFFER buffer_handle, const char c) {
    return dino_buffer_append_data(buffer_handle, &c, 1);
}

char *dino_buffer_data_ptr(BUFFER buffer_handle) {
    if (buffer_handle != NULL) {
        http_buffer_t *buffer = (http_buffer_t *) buffer_handle;

        return (char *) buffer->data;
    }

    return NULL;
}

size_t dino_buffer_data_size(BUFFER buffer_handle) {
    if (buffer_handle != NULL) {
        http_buffer_t *buffer = (http_buffer_t *) buffer_handle;

        return buffer->used;
    }

    return 0;
}