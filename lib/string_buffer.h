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

#ifndef dino_string_buffer_h
#define dino_string_buffer_h

#include <stdbool.h>

typedef struct string_buffer_struct {
    char *c_string;
    size_t pos;
    size_t size;
    int realloc_count;         // Performance metric to record the number of string re-allocations
} string_buffer_t;

// Creates a new string_buffer_t with the default chunk size
//
string_buffer_t *string_buffer_new();

// Creates a new string_buffer_t with the default string
//
string_buffer_t *string_buffer_new_with_str(const char *new_string);

// Destroys the given string_buffer_t.  Pass 1 to free_string if the underlying c string should also be freed
//
void string_buffer_delete(string_buffer_t *sb, bool free_string);

// Creates a new string_buffer_t with initial size at least the given size
//
string_buffer_t *string_buffer_new_with_size(size_t size);

// Resets the string_buffer_t to empty
//
void string_buffer_reset(string_buffer_t *sb);

// Appends the given character to the string builder
//
void string_buffer_append_char(string_buffer_t *sb, const char ch);

// Appends at most length of the given src string to the string buffer
//
void string_buffer_append_str_length(string_buffer_t *string_buffer, const char *src, size_t length);

// Appends the given src string to the string builder
//
void string_buffer_append_str(string_buffer_t *sb, const char *src);

// Appends the formatted string to the given string builder
//
void string_buffer_sprintf(string_buffer_t *sb, const char *fmt, ...);

// compare two string buffers
//
int string_buffer_strcmp(string_buffer_t *sb1, string_buffer_t *sb2);

// Returns the string_buffer_t as a regular C String
//
#define string_buffer_c_string(sb) ((sb)->c_string)

// Returns the size of the string
//
#define string_buffer_c_string_length(sb) ((sb)->pos)

#endif //dino_string_buffer_h
#pragma clang diagnostic pop