/**********************************************************************
//    Copyright (c) 2017 Henry Seurer & Samuel Kelly
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

#ifndef DINO_TEMPLATE_GENERATOR_H
#define DINO_TEMPLATE_GENERATOR_H

#include "dino_string.h"
#ifndef __unused
#define __unused
#endif

typedef enum {
    dino_template_no_error = 0,
    dino_template_invalid_input,
    dino_template_file_not_found
} dino_template_error_t;


typedef bool ( *function_string_ptr_t )(void *context_ptr,
                                        const char *symbol,
                                        dino_string_ptr value);

typedef bool ( *function_boolean_ptr_t )(void *context_ptr,
                                         const char *symbol,
                                         bool *value);

dino_template_error_t dino_template_generate_buffer(dino_string_ptr input_buffer,
                                                    dino_string_ptr output_buffer,
                                                    void *context_ptr,
                                                    function_string_ptr_t function_string_ptr,
                                                    function_boolean_ptr_t function_boolean_ptr);

dino_template_error_t  __unused dino_template_process_file(dino_string_ptr source_file,
                                                 dino_string_ptr response,
                                                 void *context_ptr,
                                                 function_string_ptr_t function_string_ptr,
                                                 function_boolean_ptr_t function_boolean_ptr);

#endif //DINO_TEMPLATE_GENERATOR_H
