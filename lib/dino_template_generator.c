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

#include <memory.h>
#include <ctype.h>
#include "dino_template_generator.h"
#include "dino_utils.h"
#include "dino_intmap.h"
#include "dino_debug.h"

#define if_stack_depth 32

typedef struct pi_template_generator_struct {
    void *context_ptr;
    function_string_ptr_t function_string_ptr;
    function_boolean_ptr_t function_boolean_ptr;
    dino_intmap_ptr symbol_map;
    dino_string_ptr output_buffer;
    int if_stack_top;
    bool if_stack[if_stack_depth];
} pi_template_generator_t;


typedef enum {
    operator_type_invalid = 0,
    operator_type_variable,
    operator_type_If,
    operator_type_Else,
    operator_type_EndIf,
    operator_type_output,
} operator_type_t;

#define OPERATOR_SYMBOL(name) operator_type_##name

void pi_template_generator_create(pi_template_generator_t *ptg_context,
                                  dino_string_ptr output_buffer,
                                  void *context_ptr,
                                  function_boolean_ptr_t function_boolean_ptr,
                                  function_string_ptr_t function_string_ptr) {

    memory_clear(ptg_context, sizeof(pi_template_generator_t));

    ptg_context->output_buffer = output_buffer;

    // These need to be in sync with operator_type enum
    //
    ptg_context->symbol_map = intmap_new(64);
    intmap_put(ptg_context->symbol_map, "If", OPERATOR_SYMBOL(If));
    intmap_put(ptg_context->symbol_map, "Else", OPERATOR_SYMBOL(Else));
    intmap_put(ptg_context->symbol_map, "EndIf", OPERATOR_SYMBOL(EndIf));
    intmap_put(ptg_context->symbol_map, "=", OPERATOR_SYMBOL(output));

    ptg_context->context_ptr = context_ptr;
    ptg_context->function_string_ptr = function_string_ptr;
    ptg_context->function_boolean_ptr = function_boolean_ptr;
}

void pi_template_generator_destroy(pi_template_generator_t *ptg_context) {

    intmap_delete(ptg_context->symbol_map);
}

static const char *pi_template_skip_white(const char *pch, const char chDelim) {
    while (isspace(*pch) || chDelim == *pch) {
        pch++;
    }

    return pch;
}

dino_string_ptr pi_template_get_symbol(const char *begin_tag, char **end_tag) {
// TODO:    ASSERT(NULL != end_tag);

    if (NULL == end_tag) {
        return NULL;
    }

    // Skip white space at start of the string.
    //
    begin_tag = pi_template_skip_white(begin_tag, ' ');

    // OK now from here on out lets determine if we see a "symbol" +-% etc or are we
    // looking an alpha numeric value?
    //
    *end_tag = (char *) begin_tag;

    // Check to see if we see symbol
    //
    if (!isalnum(**end_tag)) {
        // Huh, it must be something special, we only take symbols one at a time.
        //
        (*end_tag)++;
    } else {
        // We are good to go either this is a number of a symbol.
        while (!isspace(**end_tag) && '%' != **end_tag && **end_tag) {
            (*end_tag)++;
        }
    }

    dino_string_ptr symbol = dino_string_new_with_size(*end_tag - begin_tag);
    dino_string_append_str_length(symbol, begin_tag, *end_tag - begin_tag);

    return symbol;
}

bool pi_template_resolve_symbol(pi_template_generator_t *ptg_context,
                                dino_string_ptr symbol_buffer,
                                dino_string_ptr result_buffer) {

    operator_type_t operator_type = (operator_type_t) intmap_get_value(ptg_context->symbol_map,
                                                                       dino_string_c_ptr(symbol_buffer));

    bool valid = false;

    if (operator_type == operator_type_invalid || operator_type == operator_type_variable) {

        if (result_buffer) {
            dino_string_reset(result_buffer);
        }

        valid = (*ptg_context->function_string_ptr)(ptg_context->context_ptr,
                                                    dino_string_c_ptr(symbol_buffer),
                                                    result_buffer);

        if (valid && operator_type == operator_type_invalid) {
            intmap_put(ptg_context->symbol_map, dino_string_c_ptr(symbol_buffer), operator_type_variable);
        }

    }

    return valid;
}

bool pi_template_test_symbol(pi_template_generator_t *ptg_context,
                             dino_string_ptr symbol_buffer,
                             bool *value) {

    operator_type_t operator_type = (operator_type_t) intmap_get_value(ptg_context->symbol_map,
                                                                       dino_string_c_ptr(symbol_buffer));

    bool valid = false;

    if (operator_type == operator_type_invalid || operator_type == operator_type_variable) {

        if (value) {
            *value = false;
        }

        valid = (*ptg_context->function_boolean_ptr)(ptg_context->context_ptr,
                                                     dino_string_c_ptr(symbol_buffer),
                                                     value);

        if (valid && operator_type == operator_type_invalid) {
            intmap_put(ptg_context->symbol_map, dino_string_c_ptr(symbol_buffer), operator_type_variable);
        }

    }

    return valid;
}

operator_type_t pi_template_lookup_symbol(pi_template_generator_t *ptg_context,
                                          const char *begin_tag,
                                          dino_string_ptr result_buffer,
                                          bool *response_result) {
    ASSERT(result_buffer != NULL && response_result != NULL);
    ASSERT(*begin_tag == '<' && *(begin_tag + 1) == '%');

    if (result_buffer) {
        dino_string_reset(result_buffer);
    }

    if (response_result) {
        *response_result = false;
    }

    operator_type_t operator_type = operator_type_invalid;

    if (*begin_tag == '<' && *(begin_tag + 1) == '%') {
        //  Move past the '<%' tag marker and any leading white spaces.
        //
        char *end_tag = NULL;
        dino_string_ptr first_symbol = pi_template_get_symbol(begin_tag + 2, &end_tag);

        operator_type = (operator_type_t) intmap_get_value(ptg_context->symbol_map,
                                                           dino_string_c_ptr(first_symbol));

        dino_string_ptr second_symbol = NULL;

        switch (operator_type) {
            case operator_type_If:
                second_symbol = pi_template_get_symbol(end_tag, &end_tag);
                if (!pi_template_test_symbol(ptg_context, second_symbol, response_result)) {
                    operator_type = operator_type_invalid;
                }
                break;

            case operator_type_Else:
            case operator_type_EndIf:
                break;

            case operator_type_output:
                second_symbol = pi_template_get_symbol(end_tag, &end_tag);
                if (!pi_template_resolve_symbol(ptg_context, second_symbol, result_buffer)) {
                    operator_type = operator_type_invalid;
                }
                break;

            case operator_type_variable:
                if (!pi_template_resolve_symbol(ptg_context, first_symbol, result_buffer)) {
                    operator_type = operator_type_invalid;
                }
                break;

            case operator_type_invalid:
            default:
                ERROR_LOG("Unknown symbol or operator: %s", dino_string_c_ptr(first_symbol));
                break;

        }

        dino_string_delete(first_symbol, true);
        dino_string_delete(second_symbol, true);
    }

    return operator_type;
}

void pi_template_push_if(pi_template_generator_t *ptg_context, bool value) {
    if (ptg_context->if_stack_top < if_stack_depth) {
        ptg_context->if_stack[ptg_context->if_stack_top] = value;
    }
    ptg_context->if_stack_top++;
}

void pi_template_push_else(pi_template_generator_t *ptg_context) {
    if (ptg_context->if_stack_top > 0 && ptg_context->if_stack_top < if_stack_depth) {
        ptg_context->if_stack[ptg_context->if_stack_top - 1] =
                !ptg_context->if_stack[ptg_context->if_stack_top - 1];
    }
}

void pi_template_pop_if(pi_template_generator_t *ptg_context) {
    if (ptg_context->if_stack_top > 0) {
        ptg_context->if_stack_top--;
    }
}

bool pi_template_if_set(pi_template_generator_t *ptg_context) {
    for (int i = 0; i < ptg_context->if_stack_top; i++) {
        if (!ptg_context->if_stack[i]) {
            return false;
        }
    }
    return true;
}

void pi_template_output(pi_template_generator_t *ptg_context, const char *ptr_in) {
    if (pi_template_if_set(ptg_context)) {
        dino_string_append_str(ptg_context->output_buffer, ptr_in);
    }
}

void pi_template_output_length(pi_template_generator_t *ptg_context, const char *ptr_in, size_t length) {
    if (pi_template_if_set(ptg_context)) {
        dino_string_append_str_length(ptg_context->output_buffer, ptr_in, length);
    }
}

dino_template_error_t dino_template_generate_buffer(dino_string_ptr input_buffer,
                                                    dino_string_ptr output_buffer,
                                                    void *context_ptr,
                                                    function_string_ptr_t function_string_ptr,
                                                    function_boolean_ptr_t function_boolean_ptr) {
    if (NULL == function_string_ptr
        || NULL == function_boolean_ptr
        || NULL == output_buffer
        || NULL == input_buffer) {
        return dino_template_invalid_input;
    }

    dino_string_reset(output_buffer);

    pi_template_generator_t ptg_context;

    pi_template_generator_create(&ptg_context,
                                 output_buffer,
                                 context_ptr,
                                 function_boolean_ptr,
                                 function_string_ptr);

    const char *ptr_in = dino_string_c_ptr(input_buffer);
    const char *ptr_EOF = ptr_in + dino_string_c_strlen(input_buffer);

    if (*ptr_in == '%') {
        dino_string_append_char(output_buffer, *ptr_in);
        ptr_in++;
    }

    dino_string_ptr symbol_buffer = dino_string_new();

    while (ptr_in < ptr_EOF) {
        //  Look for '%' and then see if we have a "<%"
        //  Initialization would have ensured that we wont crash...
        //
        const char *ptr_tag = strchr(ptr_in, '%');
        if (!ptr_tag) {
            //  No more tags, copy the rest of the data to the output
            //  buffer and break the loop.
            //
            pi_template_output(&ptg_context, ptr_in);
            break;
        }

        //  Set the ptr_tag to the beginning of the '<%' tag...
        ptr_tag--;

        size_t length_to_append = ptr_tag - ptr_in + 2 * (*ptr_tag != '<');

        pi_template_output_length(&ptg_context, ptr_in, length_to_append);

        ptr_in += length_to_append;
        if (*ptr_tag != '<') {
            continue;
        }

        bool bool_value = false;
        operator_type_t operator_type = pi_template_lookup_symbol(&ptg_context,
                                                                  ptr_in,
                                                                  symbol_buffer,
                                                                  &bool_value);
        switch (operator_type) {
            case operator_type_invalid:
            default:
                ERROR_LOG("Invalid Operator Type at offset %d.", ptr_EOF - ptr_in);
                break;
            case operator_type_variable:
            case operator_type_output:
                pi_template_output(&ptg_context, dino_string_c_ptr(symbol_buffer));
                break;

            case operator_type_If:
                pi_template_push_if(&ptg_context, bool_value);
                break;

            case operator_type_Else:
                pi_template_push_else(&ptg_context);
                break;

            case operator_type_EndIf:
                pi_template_pop_if(&ptg_context);
                break;
        }

        // Skip over opening <%
        //
        if (*ptr_in == '<' && *(ptr_in + 1) == '%') {
            ptr_in += 2;
        }

        // Skip to closing %>
        //
        while ('%' != *ptr_in && *ptr_in) {
            ptr_in++;
        }

        if (*ptr_in == '%' && *(ptr_in + 1) == '>') {
            ptr_in += 2;
        }
    }
    dino_string_delete(symbol_buffer, true);

    pi_template_generator_destroy(&ptg_context);
    return dino_template_no_error;
}

size_t http_html_file_size(FILE *file_p) {
    if (NULL == file_p) {
        return 0;
    }

    fseek(file_p, 0L, SEEK_END);
    long file_size = ftell(file_p);
    fseek(file_p, 0L, SEEK_SET);

    return (size_t) file_size;
}

dino_template_error_t  __unused dino_template_process_file(dino_string_ptr source_file,
                                                 dino_string_ptr response,
                                                 void *context_ptr,
                                                 function_string_ptr_t function_string_ptr,
                                                 function_boolean_ptr_t function_boolean_ptr) {
    FILE *file_p = fopen(dino_string_c_ptr(source_file), "r");

    dino_template_error_t status = dino_template_invalid_input;

    if (file_p) {
        size_t file_size = http_html_file_size(file_p);

        if (file_size) {

            // Read in the file:
            //
            char *file_contents = memory_alloc(file_size);
            memory_clear(file_contents, file_size);

            if (fread(file_contents, file_size, sizeof(char), file_p) != 0) {
                // Setup buffers.
                //
                dino_string_ptr input_buffer = dino_string_new_with_size(file_size + 1);
                dino_string_append_str_length(input_buffer, file_contents, file_size);

                dino_string_ptr response_body = dino_string_new_with_size(file_size + 1);

                dino_template_generate_buffer(input_buffer, response_body, context_ptr, function_string_ptr,
                                              function_boolean_ptr);

                // Output the header
                //
                dino_string_sprintf(response, "HTTP/1.0 200 OK\r\n");
                dino_string_sprintf(response, "Server: %s\r\n", get_dino_version());
                dino_string_sprintf(response, "Content-Type: text/html\r\n");
                dino_string_sprintf(response, "Connection: close\r\n");
                dino_string_sprintf(response, "Content-Length: %d\r\n", dino_string_c_strlen(response_body));
                dino_string_sprintf(response, "\r\n%s", dino_string_c_ptr(response_body));

                dino_string_delete(response_body, true);
                dino_string_delete(input_buffer, true);
                status = dino_template_no_error;
            } else {
                ERROR_LOG("Unable to read file %s", dino_string_c_ptr(source_file));
                status = dino_template_file_not_found;
            }

            // Free memory used to store file.
            //
            memory_free(file_contents);

            // Close the file
            //
            fclose(file_p);
        }
    }

    return status;
}