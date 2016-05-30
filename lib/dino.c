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
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dino.h"
#include "dino_http.h"

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
                                   string_buffer_c_string(list->name),
                                   string_buffer_c_string_length(list->name))) {
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

    dino_route->name = string_buffer_new_with_size(strlen(method_name) + strlen(name));

    string_buffer_append_str(dino_route->name, method_name);
    string_buffer_append_str(dino_route->name, name);

    // Store the path
    //
    dino_route->path = string_buffer_new_with_str(path);

    // Parse the path, we need to see if there are any :[name] directives
    //
    dino_route->stack = stack_ptr_parse(dino_route->stack, string_buffer_c_string(dino_route->path), "/");

    // Save callback function pointer
    //
    dino_route->verb_func = verb_func;
    dino_route->method = method;

    return true;
}

DHANDLE DINO_EXPORT dino_config_start(char *host) {
    dino_http_site_t *dino_site = (dino_http_site_t *) memory_alloc(sizeof(dino_http_site_t));
    if (NULL != dino_site) {

        dino_site->host = string_buffer_new_with_str(host);
        dino_site->port = 80;
        dino_site->list = NULL;
        dino_site->handle_type = dino_handle_site;
    }

    return dino_site;
}

bool DINO_EXTERN dino_http_port(DHANDLE dhandle, unsigned short port, const char *function, int line){

    fprintf(stdout, "[INFO][%s:%d] Setting HTTP port to %hu \n\r", function, line, port);

    dino_http_site_t *dino_site = cast_dhandle_site(dhandle);

    dino_site->port = port;

    return true;
}


bool DINO_EXPORT dino_route_get(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                const char *function, int line) {
    if (!add_method_to_site(http_get, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] GET unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }

    return true;
}

bool DINO_EXPORT dino_route_post(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                 const char *function, int line) {
    if (!add_method_to_site(http_post, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] POST unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }

    return true;
}

bool DINO_EXPORT dino_route_delete(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                   const char *function, int line) {
    if (!add_method_to_site(http_delete, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] DELETE unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_put(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                const char *function, int line) {
    if (!add_method_to_site(http_put, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] PUT unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_options(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                    const char *function, int line) {
    if (!add_method_to_site(http_options, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] OPTIONS unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_head(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                 const char *function, int line) {
    if (!add_method_to_site(http_head, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] HEAD unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_trace(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                  const char *function, int line) {
    if (!add_method_to_site(http_trace, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] TRACE unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }
    return true;
}

bool DINO_EXPORT dino_route_connect(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path,
                                    const char *function, int line) {
    if (!add_method_to_site(http_connect, dhandle, verb_func, name, path)) {
        fprintf(stderr, "[ERROR][%s:%d] CONNECT unable to bind %s to path %s\n\r", function, line, name, path);
        return false;
    }
    return true;
}

bool g_dino_keep_running = true;

void dino_signal_shutdown(int value) {
    fprintf(stderr, "[WARNING] Shutting down the service, signal: %d\n\r", value);
    g_dino_keep_running = false;
    dino_http_stop();
    exit(value);
}

void dino_signal_SIGPIPE(int value) {
    fprintf(stderr, "[WARNING] SIGPIPE failure: %d\n\r", value);
}

bool DINO_EXPORT dino_start(DHANDLE dhandle, const char *function, int line) {
    if (NULL == dhandle) {
        fprintf(stderr, "[ERROR][%s:%d] Unable to start Dino, the dhandle is invlaid.\n\r", function, line);
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
        if (NULL == response->buffer_handle){
            response->buffer_handle = string_buffer_new_with_size(size);
        }

        string_buffer_append_str_length(response->buffer_handle, data, size);
    }
    else {
        fprintf(stderr, "[ERROR] buffer_handle is NULL... \n\r");
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
        }
        else {
            fprintf(stderr, "[ERROR] Invalid key, must have at least one character...\n\r");
        }
    }
    else {
        fprintf(stderr, "[ERROR] response is NULL... \n\r");
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


#pragma clang diagnostic pop