/**********************************************************************
//    Copyright (c) 2014 Henry Seurer
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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "dino_buffer.h"
#include "dino.h"
#include "dino_http.h"

// DHANDLE Casting helpers functions.
//

dino_site_t *cast_dhandle_site(DHANDLE dhandle) {
    if (NULL != dhandle) {
        dino_site_t *ptr = (dino_site_t *) dhandle;

        if (dino_handle_site == ptr->handle_type) {
            return ptr;
        }
    }
    return NULL;
}

http_request_t *cast_dhandle_request(DHANDLE dhandle) {
    if (NULL != dhandle) {
        http_data_t *ptr = (http_data_t *) dhandle;

        if (dino_handle_http == ptr->handle_type) {
            return &ptr->request;
        }
    }
    return NULL;
}

http_response_t *cast_dhandle_response(DHANDLE dhandle) {
    if (NULL != dhandle) {
        http_data_t *ptr = (http_data_t *) dhandle;

        if (dino_handle_http == ptr->handle_type) {
            return &ptr->response;
        }
    }
    return NULL;
}

// HTTP Verbs to function prefix strings...
//

const char *http_method_prefix_string(http_method method) {
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
    while (NULL != list && strncmp(name, list->name, strlen(list->name))) {
        list = list->next;
    }

    return list;
}

bool add_method_to_site(http_method method, DHANDLE dhandle, http_verb_func verb_func, const char *name,
                        const char *path) {
    if (NULL == dhandle) {
        return false;
    }

    dino_site_t *psite = cast_dhandle_site(dhandle);

    if (NULL != list_find(psite->list, name)) {
        return false;
    }

    dino_route_t *ptr_path = list_add_new_item(&psite->list);

    // Build name for method
    //
    const char *method_name = http_method_prefix_string(method);

    ptr_path->name = memory_alloc(strlen(method_name) + strlen(name) + 1);
    strncpy(ptr_path->name, method_name, strlen(method_name));
    strncat(ptr_path->name, name, strlen(name));

    // Store the path
    //
    ptr_path->path = memory_alloc(strlen(path) + 1);
    strncpy(ptr_path->path, path, strlen(path));

    // Parse the path, we need to see if there are any :[name] directives
    //
    ptr_path->stack = stack_ptr_parse(ptr_path->stack, ptr_path->path, "/");

    // Save callback function pointer
    //
    ptr_path->verb_func = verb_func;
    ptr_path->method = method;

    return true;
}

DHANDLE DINO_EXPORT dino_config_start(unsigned short port, char *host) {
    dino_site_t *psite = (dino_site_t *) memory_alloc(sizeof(dino_site_t));
    if (NULL != psite) {
        psite->host = memory_alloc(strlen(host) + 1);
        strncpy(psite->host, host, strlen(host));

        psite->port = port;
        psite->list = NULL;
        psite->handle_type = dino_handle_site;
    }

    return psite;
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
    dino_stop_http();
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

    dino_start_http(cast_dhandle_site(dhandle));

    return true;
}

void DINO_EXPORT response_send(DHANDLE dhandle, const char *data, size_t size) {
    http_response_t *response = cast_dhandle_response(dhandle);

    if (NULL != response) {
        response->buffer_handle = buffer_append_data(response->buffer_handle, data, size);
    }
    else {
        fprintf(stderr, "[ERROR] resposne_handle is NULL... \n\r");
    }
}

int DINO_EXPORT response_printf(DHANDLE dhandle, const char *fmt, ...) {
    // Points to each unnamed arg in turn
    //
    va_list ap;

    // make ap point to 1st unnamed arg
    //
    va_start(ap, fmt);

    // vasprintf with variable params
    //
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
    http_response_t *response = cast_dhandle_response(dhandle);

    if (NULL != response) {
        if (key && *key) {
            dino_sm_add(response->params_map, key, value);
        }
        else {
            fprintf(stderr, "[ERROR] Invlaid key, must have at least one character...\n\r");
        }
    }
    else {
        fprintf(stderr, "[ERROR] resposne_handle is NULL... \n\r");
    }
}

const char *DINO_EXPORT params_get(DHANDLE dhandle, const char *key) {
    http_request_t *request = cast_dhandle_request(dhandle);

    if (NULL != request) {
        return dino_sm_get_value(request->params_map, key);
    }

    return "";
}

size_t DINO_EXPORT params_count(DHANDLE dhandle) {
    http_request_t *request = cast_dhandle_request(dhandle);
    if (NULL != request) {
        return (size_t) dino_sm_get_count(request->params_map);
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
    http_request_t *request = cast_dhandle_request(dhandle);

    if (NULL != request) {
        dino_callback_param_t callback_data;
        memory_clear(&callback_data, sizeof(dino_callback_param_t));
        callback_data.callback = callback;
        callback_data.dhandle = dhandle;
        callback_data.obj = obj;

        dino_sm_enum(request->params_map, param_enum, &callback_data);

    }
}


