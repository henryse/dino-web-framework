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

#ifndef dino_dino_http_h
#define dino_dino_http_h

#include "dino_utils.h"
#include "dino_strmap.h"

// List of methods
//
typedef enum http_method_enum {
    http_invalid,
    http_get,
    http_post,
    http_delete,
    http_put,
    http_options,
    http_head,
    http_trace,
    http_connect
} http_method;

// Route describing a "path"
//
typedef struct dino_route_struct {
    struct dino_route_struct *next;
    http_method method;
    http_verb_func verb_func;
    char *name;
    char *path;
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
typedef struct dino_site_struct {
    dino_handle_type_t handle_type;

    unsigned short port;
    dino_route_t *list;
    char *host;
} dino_site_t;

// Request structure
//
typedef struct http_request_struct {
    size_t content_size;
    char *data;

    http_method method;
    char *url;

    str_map_t *params_map;
} http_request_t;

// Response structure
//
typedef struct http_response_struct {
    BUFFER buffer_handle;

    str_map_t *params_map;
} http_response_t;

typedef struct http_data_struct {
    dino_handle_type_t handle_type;

    int socket;

    http_request_t request;
    http_response_t response;
} http_data_t;

typedef union {
    http_data_t http;
} dino_handle_t;

// Start server
//
void dino_start_http(dino_site_t *psite);

// Stop Server
void dino_stop_http();

extern bool g_dino_keep_running;
#endif
