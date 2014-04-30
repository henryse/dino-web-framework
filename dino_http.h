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
#define HOST_STRING_SIZE 1024
#define MAX_HTTP_HEADER_LINE_LENGTH 1024
#define MAX_ERROR_LENGTH 1024
#define HTTP_MAX_PARAMS 32
#define HTTP_MAX_KEY_SIZE 128
#define HTTP_MAX_VALUE_SIZE 128
#define HTTP_URL_SIZE 1024

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

// Site
//
typedef struct dino_site_struct
{
    u_short port;
    char host[HOST_STRING_SIZE];
    dino_route *list;
}dino_site;

// HTTP Parametgers/key value paire.
//
typedef struct http_key_value_struct
{
    char key[HTTP_MAX_KEY_SIZE];
    char value[HTTP_MAX_VALUE_SIZE];
}http_key_value;

// Request structure
//
typedef struct http_request_struct
{
    http_key_value params[HTTP_MAX_PARAMS];
    int param_index;
    
    http_method method;
    char url[HTTP_URL_SIZE];

    int socket;
    
    size_t content_size;
    char *data;
}http_request;

// Response structure
//
typedef struct http_response_struct
{
    int client;
    BUFFER buffer_handle;

    http_key_value params[HTTP_MAX_PARAMS];
    int param_index;
}http_response;

// Start server
//
void dino_start_http(dino_site *psite);

#endif
