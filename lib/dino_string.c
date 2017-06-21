/**********************************************************************
//    Copyright (c) 2017 Henry Seurer
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
#include <string.h>
#include <stdarg.h>
#include "dino_string.h"
#include "dino_utils.h"

dino_string_ptr dino_string_new() {
    return dino_string_new_with_size(64);
}

dino_string_ptr dino_string_new_with_str(const char *new_string) {
    size_t length = strlen(new_string);

    dino_string_ptr buffer = dino_string_new_with_size(length);

    dino_string_append_str(buffer, new_string);

    return buffer;
}

dino_string_ptr dino_string_new_with_size(size_t size) {
    dino_string_ptr dino_string = (dino_string_ptr) memory_alloc(sizeof(dino_string_t));
    if (dino_string) {
        memory_clear(dino_string, sizeof(dino_string_t));

        dino_string->size = size;
        dino_string->c_string = (char *) memory_alloc(size);
        memory_clear(dino_string->c_string, size);

        dino_string->position = 0;
    }

    return dino_string;
}

void dino_string_reset(dino_string_ptr dino_string) {
    dino_string->position = 0;
    memory_clear(dino_string->c_string, dino_string->size);
}

void dino_string_delete(dino_string_ptr dino_string, bool free_string) {
    if (dino_string) {
        if (free_string) {
            memory_clear(dino_string->c_string, dino_string->size);
            memory_free(dino_string->c_string);
        }

        memory_clear(dino_string, sizeof(dino_string_t));
        memory_free(dino_string);
    }
}

bool dino_string_resize(dino_string_ptr dino_string, const size_t new_size) {
    char *old_c_string = dino_string->c_string;

    dino_string->c_string = (char *) memory_realloc(dino_string->c_string, new_size);
    if (dino_string->c_string == NULL) {
        dino_string->c_string = old_c_string;
        return false;
    }
    memory_clear(dino_string->c_string + dino_string->position, new_size - dino_string->position);

    dino_string->size = new_size;
    return true;
}

int dino_string_buffer_double_size(dino_string_ptr dino_string) {
    return dino_string_resize(dino_string, dino_string->size * 2);
}

void dino_string_append_char(dino_string_ptr dino_string, const char ch) {
    if (dino_string->position == dino_string->size) {
        dino_string_buffer_double_size(dino_string);
    }

    dino_string->c_string[dino_string->position++] = ch;
}

void dino_string_append_str_length(dino_string_ptr dino_string, const char *src, size_t length) {
    size_t chars_remaining;
    size_t chars_required;
    size_t new_size;

    // <buffer size> - <zero based index of next char to write> - <space for null terminator>
    chars_remaining = dino_string->size - dino_string->position - 1;
    if (chars_remaining < length) {
        chars_required = length - chars_remaining;
        new_size = dino_string->size;
        do {
            new_size = new_size * 2;
        } while (new_size < (dino_string->size + chars_required));
        dino_string_resize(dino_string, new_size);
    }

    memcpy(dino_string->c_string + dino_string->position, src, length);
    dino_string->position += length;
}

void dino_string_append_str(dino_string_ptr dino_string, const char *src) {
    dino_string_append_str_length(dino_string, src, strlen(src));
}

void dino_string_sprintf(dino_string_ptr dino_string, const char *template, ...) {
    char *str;
    va_list arg_list;

    va_start(arg_list, template);
    vasprintf(&str, template, arg_list);
    va_end(arg_list);

    if (!str) {
        return;
    }

    dino_string_append_str(dino_string, str);
    memory_free(str);
}

int dino_string_strcmp(dino_string_ptr dino_string_1, dino_string_ptr dino_string_2) {
    // If they are both NULL then I guess they are "equal"
    //
    if (dino_string_1 == NULL && dino_string_2 == NULL) {
        return 0;
    }

    // If they are one is NULL then I guess they are "not equal"
    //
    if (dino_string_1 == NULL) {
        return -1;
    }

    if (dino_string_2 == NULL) {
        return 1;
    }

    return strncmp(dino_string_c_ptr(dino_string_1),
                   dino_string_c_ptr(dino_string_2),
                   max(dino_string_c_strlen(dino_string_1),
                       dino_string_c_strlen(dino_string_2)));
}

char *dino_string_c_ptr(dino_string_ptr string_ptr) {
    char *empty_string = "";

    if (string_ptr == NULL || string_ptr->c_string == NULL) {
        return empty_string;
    }

    return (string_ptr)->c_string;
}

size_t dino_string_c_strlen(dino_string_ptr string_ptr) {
    if (string_ptr == NULL || string_ptr->c_string == NULL) {
        return 0;
    }

    return string_ptr->position;
}


#pragma clang diagnostic pop