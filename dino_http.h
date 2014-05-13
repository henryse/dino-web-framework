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

#ifndef dino_dino_http_h
#define dino_dino_http_h
#include "dino_utils.h"

// HTTP Defines used:
//
#define HTTP_MAX_PARAMS 32
#define HTTP_MAX_KEY_SIZE 128
#define HTTP_MAX_VALUE_SIZE 128

// List of methods
//
typedef enum http_method_enum
{
    http_invalid,
    http_get,
    http_post,
    http_delete,
    http_put,
    http_options,
    http_head,
    http_trace,
    http_connect
}http_method;

// Route describing a "path"
//
typedef struct dino_route_struct
{
    struct dino_route_struct *next;
    http_method method;
    http_verb_func verb_func;
    char *name;
    char *path;
    stack_char_ptr *stack;
}dino_route;

// HTTP Parametgers/key value paire.
//
typedef struct http_key_value_struct
{
    char key[HTTP_MAX_KEY_SIZE];
    char value[HTTP_MAX_VALUE_SIZE];
}http_key_value;

typedef enum dino_handle_type_enum
{
    dino_handle_invalid,
    dino_handle_site,
    dino_handle_http,
}dino_handle_type;

// Site
//
typedef struct dino_site_struct
{
    dino_handle_type handle_type;

    u_short port;
    dino_route *list;
    char *host;
}dino_site;

// Request structure
//
typedef struct http_request_struct
{    
    size_t content_size;
    char *data;

    http_method method;
    char *url;

    int param_index;
    http_key_value params[HTTP_MAX_PARAMS];
}http_request;

// Response structure
//
typedef struct http_response_struct
{
    BUFFER buffer_handle;

    int param_index;
    http_key_value params[HTTP_MAX_PARAMS];
}http_response;

typedef struct http_data_struct
{
    dino_handle_type handle_type;

    int socket;

    http_request request;
    http_response response;
}http_data;

typedef union dino_handle_type
{
    dino_site site;
    http_data http;
}dino_handle;

// Start server
//
void dino_start_http(dino_site *psite);

// Stop Server
void dino_stop_http();

extern bool g_dino_keep_running;
#endif
