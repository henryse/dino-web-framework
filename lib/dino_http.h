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

#ifndef dino_dino_http_h
#define dino_dino_http_h

#include "dino_utils.h"
#include "dino_strmap.h"
#include "dino_string.h"

// List of methods
//
typedef enum dino_http_method_enum {
    http_invalid,
    http_get,
    http_post,
    http_delete,
    http_put,
    http_options,
    http_head,
    http_trace,
    http_connect
} dino_http_method;

// Route describing a "path"
//
typedef struct dino_route_struct {
    struct dino_route_struct *next;
    dino_http_method method;
    http_verb_func verb_func;
    dino_string_ptr name;
    dino_string_ptr path;
    stack_char_ptr_t *stack;
} dino_route_t;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
typedef enum dino_handle_type_enum {
    dino_handle_invalid,
    dino_handle_site,
    dino_handle_http,
} dino_handle_type_t;
#pragma clang diagnostic pop

// Site
//
typedef struct dino_http_site_struct {
    dino_handle_type_t handle_type;

    unsigned short port;
    dino_route_t *list;
    dino_string_t  __unused *host;
} dino_http_site_t;

// Request structure
//
typedef struct dino_http_request_struct {
    size_t content_size;
    char *data;

    dino_http_method method;
    dino_string_ptr url;

    str_map_t *params_map;
} dino_http_request_t;

// Response structure
//
typedef struct dino_http_response_struct {
    dino_string_ptr buffer_handle;

    str_map_t *params_map;
} dino_http_response_t;

typedef struct dino_http_data_struct {
    dino_handle_type_t handle_type;

    int socket;

    dino_http_request_t request;
    dino_http_response_t response;
} dino_http_data_t;

typedef union {
    dino_http_data_t http;
} dino_handle_t;

// Start server
//
void dino_http_start(dino_http_site_t *dino_site);

// Stop Server
void dino_http_stop();

extern bool g_dino_keep_running;
#endif
