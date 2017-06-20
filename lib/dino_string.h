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

#ifndef dino_string_h
#define dino_string_h

#include <stdbool.h>

typedef struct string_buffer_struct {
    char *c_string;
    size_t position;
    size_t size;
} dino_string_t;

typedef dino_string_t *dino_string_ptr;

// Creates a new dino_string_t with the default chunk size
//
dino_string_ptr dino_string_new();

// Creates a new dino_string_t with the default string
//
dino_string_ptr dino_string_new_with_str(const char *new_string);

// Destroys the given dino_string_t.  Pass 1 to free_string if the underlying c string should also be freed
//
void dino_string_delete(dino_string_ptr sb, bool free_string);

// Creates a new dino_string_t with initial size at least the given size
//
dino_string_ptr dino_string_new_with_size(size_t size);

// Resets the dino_string_t to empty
//
void dino_string_reset(dino_string_ptr dino_string);

// Appends the given character to the string builder
//
void dino_string_append_char(dino_string_ptr dino_string, const char ch);

// Appends at most length of the given src string to the string buffer
//
void dino_string_append_str_length(dino_string_ptr dino_string, const char *src, size_t length);

// Appends the given src string to the string builder
//
void dino_string_append_str(dino_string_ptr dino_string, const char *src);

// Appends the formatted string to the given string builder
//
void dino_string_sprintf(dino_string_ptr dino_string, const char *fmt, ...);

// compare two string buffers
//
int dino_string_strcmp(dino_string_ptr dino_string_1, dino_string_ptr dino_string_2);

// Returns the dino_string_t as a regular C String
//
char *dino_string_c_ptr(dino_string_ptr string_ptr);

// Returns the size of the string
//
size_t dino_string_c_strlen(dino_string_ptr string_ptr);

#endif //dino_string_h
#pragma clang diagnostic pop