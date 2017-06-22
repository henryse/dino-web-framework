//
// Created by Henry Seurer on 6/21/17.
//

#ifndef DINO_TEMPLATE_TYPES_H
#define DINO_TEMPLATE_TYPES_H

typedef enum {
    dino_template_no_error = 0,
    dino_template_invalid_input,
    dino_template_file_not_found
} dino_template_error_t;


typedef bool ( *function_string_ptr_t )(void *context_ptr,
                                        const char *symbol,
                                        char **value);

typedef bool ( *function_boolean_ptr_t )(void *context_ptr,
                                         const char *symbol,
                                         bool *value);

#endif //DINO_TEMPLATE_TYPES_H
