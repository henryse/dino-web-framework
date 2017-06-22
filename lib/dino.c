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
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// #include <openssl/ssl.h>
// #include <openssl/err.h>
#include "dino.h"
#include "dino_http.h"
#include "dino_debug.h"
#include "dino_template_generator.h"

// DHANDLE Casting helpers functions.
//

dino_http_site_t *cast_dhandle_site(DHANDLE dhandle) {
    if (NULL != dhandle) {
        dino_http_site_t *ptr = (dino_http_site_t *) dhandle;

        if (dino_handle_site == ptr->handle_type) {
            return ptr;
        }
    }
    return NULL;
}

dino_http_request_t *cast_dhandle_request(DHANDLE dhandle) {
    if (NULL != dhandle) {
        dino_http_data_t *ptr = (dino_http_data_t *) dhandle;

        if (dino_handle_http == ptr->handle_type) {
            return &ptr->request;
        }
    }
    return NULL;
}

dino_http_response_t *cast_dhandle_response(DHANDLE dhandle) {
    if (NULL != dhandle) {
        dino_http_data_t *ptr = (dino_http_data_t *) dhandle;

        if (dino_handle_http == ptr->handle_type) {
            return &ptr->response;
        }
    }
    return NULL;
}

// HTTP Verbs to function prefix strings...
//

const char *http_method_prefix_string(dino_http_method method) {
    switch (method) {
        case http_get:
            return "get_";
        case http_post:
            return "post_";
        case http_delete:
            return "delete_";
        case http_put:
            return "put_";
        case http_options:
            return "options_";
        case http_head:
            return "head_";
        case http_trace:
            return "trace_";
        case http_connect:
            return "connect_";
        default:
            break;
    }
    return "unknown_";
}

dino_route_t *list_add_new_item(dino_route_t **head) {
    // List is invalid.
    //
    if (NULL == head) {
        return NULL;
    }

    // List is empty.
    //
    if (NULL == *head) {
        *head = (dino_route_t *) memory_alloc(sizeof(dino_route_t));
        return *head;
    }

    // Add to the end of the list.
    //
    dino_route_t *ptr = *head;

    while (ptr->next != NULL) { ptr = ptr->next; }

    ptr->next = (dino_route_t *) memory_alloc(sizeof(dino_route_t));

    return ptr->next;
}

dino_route_t *list_find(dino_route_t *list, const char *name) {
    while (NULL != list && strncmp(name,
                                   dino_string_c_ptr(list->name),
                                   dino_string_c_strlen(list->name))) {
        list = list->next;
    }

    return list;
}

bool add_method_to_site(dino_http_method method, DHANDLE dhandle, http_verb_func verb_func, const char *name,
                        const char *path) {
    if (NULL == dhandle) {
        return false;
    }

    dino_http_site_t *dino_site = cast_dhandle_site(dhandle);

    if (NULL != list_find(dino_site->list, name)) {
        return false;
    }

    dino_route_t *dino_route = list_add_new_item(&dino_site->list);

    // Build name for method
    //
    const char *method_name = http_method_prefix_string(method);

    dino_route->name = dino_string_new_with_size(strlen(method_name) + strlen(name));

    dino_string_append_str(dino_route->name, method_name);
    dino_string_append_str(dino_route->name, name);

    // Store the path
    //
    dino_route->path = dino_string_new_with_str(path);

    // Parse the path, we need to see if there are any :[name] directives
    //
    dino_route->stack = stack_ptr_parse(dino_route->stack, dino_string_c_ptr(dino_route->path), "/");

    // Save callback function pointer
    //
    dino_route->verb_func = verb_func;
    dino_route->method = method;

    return true;
}

DHANDLE DINO_EXPORT dino_config_start_http(const char *application_name, const char *host, bool enable_logging,
                                           const char *function, const char *file, int line) {
    set_application_name(application_name);
    set_log_mode(enable_logging);

    log_message(LOG_INFO, function, file, line, "Config Start with host %s.", host);

    dino_http_site_t *dino_site = (dino_http_site_t *) memory_alloc(sizeof(dino_http_site_t));
    if (NULL != dino_site) {

        dino_site->host = dino_string_new_with_str(host);
        dino_site->port = 80;
        dino_site->list = NULL;
        dino_site->handle_type = dino_handle_site;
    }

    return dino_site;
}

DHANDLE DINO_EXPORT dino_config_start_https(const char *application_name, const char *host, bool enable_logging,
                                            const char *function, const char *file, int line) {
    set_application_name(application_name);
    set_log_mode(enable_logging);

    log_message(LOG_INFO, function, file, line, "Config Start with host %s.", host);

    dino_http_site_t *dino_site = (dino_http_site_t *) memory_alloc(sizeof(dino_http_site_t));
    if (NULL != dino_site) {

        dino_site->host = dino_string_new_with_str(host);
        dino_site->port = 443;
        dino_site->list = NULL;
//        SSL_CTX *ctx = SSL_CTX_new (SSLv23_server_method ());
//        SSL_CTX_set_options (ctx,
//                             SSL_OP_SINGLE_DH_USE |
//                             SSL_OP_SINGLE_ECDH_USE |
//                             SSL_OP_NO_SSLv2);
        dino_site->handle_type = dino_handle_site;
    }

    return dino_site;
}

bool DINO_EXTERN dino_http_port(DHANDLE dhandle, unsigned short port,
                                const char *function, const char *file, int line) {
    log_message(LOG_INFO, function, file, line, "Setting HTTP port to %hu", port);

    dino_http_site_t *dino_site = cast_dhandle_site(dhandle);

    dino_site->port = port;

    return true;
}


bool DINO_EXPORT dino_route_get(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                const char *function, const char *file, int line) {
    if (!add_method_to_site(http_get, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "GET unable to bind %s to path %s", name, path);
        return false;
    }

    return true;
}

bool DINO_EXPORT dino_route_post(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                 const char *function, const char *file, int line) {
    if (!add_method_to_site(http_post, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "POST unable to bind %s to path %s", name, path);
        return false;
    }

    return true;
}

bool DINO_EXPORT dino_route_delete(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                   const char *function, const char *file, int line) {
    if (!add_method_to_site(http_delete, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "DELETE unable to bind %s to path %s", name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_put(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                const char *function, const char *file, int line) {
    if (!add_method_to_site(http_put, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "PUT unable to bind %s to path %s", name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_options(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                    const char *function, const char *file, int line) {
    if (!add_method_to_site(http_options, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "OPTIONS unable to bind %s to path %s", name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_head(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                 const char *function, const char *file, int line) {
    if (!add_method_to_site(http_head, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "HEAD unable to bind %s to path %s", name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_trace(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                  const char *function, const char *file, int line) {
    if (!add_method_to_site(http_trace, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "TRACE unable to bind %s to path %s", name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_connect(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                    const char *function, const char *file, int line) {
    if (!add_method_to_site(http_connect, dhandle, verb_func, name, path)) {
        log_message(LOG_ERR, function, file, line, "CONNECT unable to bind %s to path %s", name, path);
        return false;
    }
    return true;
}

bool g_dino_keep_running = true;

void dino_signal_shutdown(int value) {
    log_message(LOG_WARNING, __FUNCTION__, __FILE__, __LINE__, "Shutting down the service, signal: %d\n\r", value);
    g_dino_keep_running = false;
    dino_http_stop();
    exit(value);
}

void dino_signal_SIGPIPE(int value) {
    log_message(LOG_WARNING, __FUNCTION__, __FILE__, __LINE__, "SIGPIPE failure: %d\n\r", value);
}

bool DINO_EXPORT dino_start(DHANDLE dhandle, const char *function, const char *file, int line) {
    if (NULL == dhandle) {
        log_message(LOG_ERR, function, file, line, "Unable to start Dino, the dhandle is invalid");
        return false;
    }

    signal(SIGABRT, dino_signal_shutdown);
    signal(SIGFPE, dino_signal_shutdown);
    signal(SIGILL, dino_signal_shutdown);
    signal(SIGINT, dino_signal_shutdown);
    signal(SIGSEGV, dino_signal_shutdown);
    signal(SIGTERM, dino_signal_shutdown);
    signal(SIGPIPE, dino_signal_SIGPIPE);

    dino_http_start(cast_dhandle_site(dhandle));

    return true;
}

void DINO_EXPORT response_send(DHANDLE dhandle, const char *data, size_t size) {
    dino_http_response_t *response = cast_dhandle_response(dhandle);

    if (NULL != response) {
        if (NULL == response->buffer_handle) {
            response->buffer_handle = dino_string_new_with_size(size);
        }

        dino_string_append_str_length(response->buffer_handle, data, size);
    } else {
        log_message(LOG_ERR, __FUNCTION__, __FILE__, __LINE__, "buffer_handle is NULL");
    }
}

int DINO_EXPORT response_printf(DHANDLE dhandle, const char *fmt, ...) {
    // Points to each unnamed arg in turn
    //
    va_list ap;

    // make ap point to 1st unnamed arg
    //
    va_start(ap, fmt);

    char *result = NULL;
    if (vasprintf(&result, fmt, ap) > 0) {
        if (NULL != result) {
            // now call the send method
            //
            response_send(dhandle, result, strlen(result));

            // Free the memory
            memory_free(result);
        }
    }

    // clean up when done
    //
    va_end(ap);

    return 0;
}

void DINO_EXPORT response_header_set(DHANDLE dhandle, const char *key, const char *value) {
    dino_http_response_t *response = cast_dhandle_response(dhandle);

    if (NULL != response) {
        if (key && *key) {
            dino_strmap_add(response->params_map, key, value);
        } else {
            log_message(LOG_ERR, __FUNCTION__, __FILE__, __LINE__, "Invalid key, must have at least one character...");
        }
    } else {
        log_message(LOG_ERR, __FUNCTION__, __FILE__, __LINE__, "Response is NULL...");
    }
}

const char *DINO_EXPORT params_get(DHANDLE dhandle, const char *key) {
    dino_http_request_t *request = cast_dhandle_request(dhandle);

    if (NULL != request) {
        return dino_strmap_get_value(request->params_map, key);
    }

    return "";
}

size_t DINO_EXPORT params_count(DHANDLE dhandle) {
    dino_http_request_t *request = cast_dhandle_request(dhandle);
    if (NULL != request) {
        return (size_t) dino_strmap_get_count(request->params_map);
    }

    return 0;
}

typedef struct dino_callback_param_struct {
    DHANDLE dhandle;
    dino_enum_func callback;
    const void *obj;
} dino_callback_param_t;

bool param_enum(const char *key, const char *value, const void *obj) {
    if (NULL == obj) {
        return false;
    }
    dino_callback_param_t *param_callback = (dino_callback_param_t *) obj;
    return param_callback->callback(param_callback->dhandle, key, value, param_callback->obj);
}

void DINO_EXPORT params_enumerate(DHANDLE dhandle, dino_enum_func callback, const void *obj) {
    dino_http_request_t *request = cast_dhandle_request(dhandle);

    if (NULL != request) {
        dino_callback_param_t callback_data;
        memory_clear(&callback_data, sizeof(dino_callback_param_t));
        callback_data.callback = callback;
        callback_data.dhandle = dhandle;
        callback_data.obj = obj;

        dino_strmap_enum(request->params_map, param_enum, &callback_data);

    }
}

dino_template_error_t DINO_EXTERN template_generate_buffer(const char *input_buffer,
                                                           char **output_buffer,
                                                           void *context_ptr,
                                                           function_string_ptr_t function_string_ptr,
                                                           function_boolean_ptr_t function_boolean_ptr,
                                                           const char *function, const char *file, int line) {
    log_message(LOG_INFO, function, file, line, "dino_template_generate_buffer");

    dino_string_ptr input_buffer_ptr = dino_string_new_with_str(input_buffer);
    dino_string_ptr output_buffer_ptr = dino_string_new();

    dino_template_error_t error = dino_template_generate_buffer(input_buffer_ptr,
                                                                output_buffer_ptr,
                                                                context_ptr,
                                                                function_string_ptr,
                                                                function_boolean_ptr);

    if (output_buffer) {
        *output_buffer = dino_string_c_ptr(output_buffer_ptr);
        dino_string_delete(output_buffer_ptr, false);
    } else {
        dino_string_delete(output_buffer_ptr, true);
    }

    return error;
}

dino_template_error_t DINO_EXTERN template_process_file(const char *source_file,
                                                        char **output_buffer,
                                                        void *context_ptr,
                                                        function_string_ptr_t function_string_ptr,
                                                        function_boolean_ptr_t function_boolean_ptr,
                                                        const char *function, const char *file, int line) {
    log_message(LOG_INFO, function, file, line, "dino_template_process_file");

    dino_string_ptr source_file_ptr = dino_string_new_with_str(source_file);
    dino_string_ptr output_buffer_ptr = dino_string_new();

    dino_template_error_t error = dino_template_process_file(source_file_ptr,
                                                             output_buffer_ptr,
                                                             context_ptr,
                                                             function_string_ptr,
                                                             function_boolean_ptr);

    dino_string_delete(source_file_ptr, true);

    if (output_buffer) {
        *output_buffer = dino_string_c_ptr(output_buffer_ptr);
        dino_string_delete(output_buffer_ptr, false);
    } else {
        dino_string_delete(output_buffer_ptr, true);
    }

    return error;
}

#pragma clang diagnostic pop