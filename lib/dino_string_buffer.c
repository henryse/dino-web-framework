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
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dino_string_buffer.h"
#include "dino_utils.h"

string_buffer_t *string_buffer_new() {
    return string_buffer_new_with_size(64);
}

string_buffer_t *string_buffer_new_with_str(const char *new_string) {
    size_t length = strlen(new_string);

    string_buffer_t * buffer = string_buffer_new_with_size(length);

    string_buffer_append_str(buffer, new_string);

    return buffer;
}

string_buffer_t *string_buffer_new_with_size(size_t size) {
    string_buffer_t *string_buffer = (string_buffer_t *) memory_alloc(sizeof(string_buffer_t));
    if (string_buffer) {
        memory_clear(string_buffer, sizeof(string_buffer_t));

        string_buffer->size = size;
        string_buffer->c_string = (char *) memory_alloc(size);
        memory_clear(string_buffer->c_string, size);

        string_buffer->pos = 0;
        string_buffer->realloc_count = 0;
    }

    return string_buffer;
}

void string_buffer_reset(string_buffer_t *string_buffer) {
    string_buffer->pos = 0;
    memory_clear(string_buffer->c_string, string_buffer->size);
}

void string_buffer_delete(string_buffer_t *string_buffer, bool free_string) {

    if (free_string) {
        memory_clear(string_buffer->c_string, string_buffer->size);
        memory_free(string_buffer->c_string);
    }

    memory_clear(string_buffer, sizeof(string_buffer_t));
    memory_free(string_buffer);
}

bool string_buffer_resize(string_buffer_t *string_buffer, const size_t new_size) {
    char *old_c_string = string_buffer->c_string;

    string_buffer->c_string = (char *) realloc(string_buffer->c_string, new_size);
    if (string_buffer->c_string == NULL) {
        string_buffer->c_string = old_c_string;
        return false;
    }
    memory_clear(string_buffer->c_string + string_buffer->pos, new_size - string_buffer->pos);

    string_buffer->size = new_size;
    string_buffer->realloc_count++;
    return true;
}

int string_buffer_double_size(string_buffer_t *string_buffer) {
    return string_buffer_resize(string_buffer, string_buffer->size * 2);
}

void string_buffer_append_char(string_buffer_t *string_buffer, const char ch) {
    if (string_buffer->pos == string_buffer->size) {
        string_buffer_double_size(string_buffer);
    }

    string_buffer->c_string[string_buffer->pos++] = ch;
}

void string_buffer_append_str_length(string_buffer_t *string_buffer, const char *src, size_t length) {
    size_t chars_remaining;
    size_t chars_required;
    size_t new_size;

    // <buffer size> - <zero based index of next char to write> - <space for null terminator>
    chars_remaining = string_buffer->size - string_buffer->pos - 1;
    if (chars_remaining < length) {
        chars_required = length - chars_remaining;
        new_size = string_buffer->size;
        do {
            new_size = new_size * 2;
        } while (new_size < (string_buffer->size + chars_required));
        string_buffer_resize(string_buffer, new_size);
    }

    memcpy(string_buffer->c_string + string_buffer->pos, src, length);
    string_buffer->pos += length;
}

void string_buffer_append_str(string_buffer_t *string_buffer, const char *src) {
    string_buffer_append_str_length(string_buffer, src, strlen(src));
}

void string_buffer_sprintf(string_buffer_t *string_buffer, const char *template, ...) {
    char *str;
    va_list arg_list;

    va_start(arg_list, template);
    vasprintf(&str, template, arg_list);
    va_end(arg_list);

    if (!str) {
        return;
    }

    string_buffer_append_str(string_buffer, str);
    free(str);
}

int string_buffer_strcmp(string_buffer_t *string_buffer_1, string_buffer_t *string_buffer_2) {
    // If they are both NULL then I guess they are "equal"
    //
    if (string_buffer_1 == NULL && string_buffer_2 == NULL) {
        return 0;
    }

    // If they are one is NULL then I guess they are "not equal"
    //
    if (string_buffer_1 == NULL) {
        return -1;
    }

    if (string_buffer_2 == NULL) {
        return 1;
    }

    return strncmp(string_buffer_c_string(string_buffer_1),
                   string_buffer_c_string(string_buffer_2),
                   max(string_buffer_c_string_length(string_buffer_1),
                       string_buffer_c_string_length(string_buffer_2)));
}
#pragma clang diagnostic pop