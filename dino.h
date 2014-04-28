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

#ifndef dino_dino_h
#define dino_dino_h
#include <stdbool.h>
#include "http_error.h"

// Types
//
typedef void *DHANDLE;
typedef void *REQUEST;
typedef void *RESPONE;
typedef int (*http_verb_func)(REQUEST, RESPONE);

// Funcions used by macros below.
//
DHANDLE dino_config_start(unsigned int port,  char *host);
bool dino_route_get     (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_post    (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_delete  (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_put     (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_options (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_head    (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_trace   (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);
bool dino_route_connect (DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line);

bool dino_start(DHANDLE dhandle, const char *function, int line);
bool dino_stop(DHANDLE dhandle, const char *function, int line);

void response_send      (RESPONE response_handle, const char *data, size_t size, const char *function, int line);
void response_header_set(RESPONE response_handle, const char *key, const char *value, const char *function, int line);

const char *params_get  (REQUEST request, const char *key);
const char *param_key   (REQUEST request, size_t index);
const char *param_value (REQUEST request, size_t index);
size_t      params_count(REQUEST request);

// Bindings for Routes
//
#define DINO_CONFIG_START(port, host) DHANDLE dhandle = NULL; \
            do {  \
                dhandle = dino_config_start(port, host);

#define ROUTE_GET(name, path)      if (!dino_route_get(dhandle, get_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_POST(name, path)     if (!dino_route_post(dhandle, post_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_DELETE(name, path)   if (!dino_route_delete(dhandle, delete_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_PUT(name, path)      if (!dino_route_put(dhandle, put_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_OPTIONS(name, path)  if (!dino_route_options(dhandle, options_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_HEAD(name, path)     if (!dino_route_head(dhandle, head_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_TRACE(name, path)    if (!dino_route_trace(dhandle, trace_##name, #name, path, __FUNCTION__, __LINE__)) break;
#define ROUTE_CONNECT(name, path)  if (!dino_route_connect(dhandle, connect_##name, #name, path, __FUNCTION__, __LINE__)) break;

#define DINO_CONFIG_END } while(0)

#define DINO_START dino_start(dhandle, __FUNCTION__, __LINE__);
#define DINO_STOP dino_stop(dhandle, __FUNCTION__, __LINE__);

// Declarations for Methods to be invoked for a given route.
//
#define GET(name)       int get_##name(REQUEST request, RESPONE response)
#define POST(name)      int post_##name(REQUEST request, RESPONE response)
#define PUT(name)       int put_##name(REQUEST request, RESPONE response)
#define DELETE(name)    int delete_##name(REQUEST request, RESPONE response)
#define OPTIONS(name)   int options_##name(REQUEST request, RESPONE response)
#define HEAD(name)      int head_##name(REQUEST request, RESPONE response)
#define TRACE(name)     int trace_##name(REQUEST request, RESPONE response)
#define CONNECT(name)   int connect_##name(REQUEST request, RESPONE response)

// Operations
//
#define RESPONSE_SEND(data, size)  response_send(response, data, size, __FUNCTION__, __LINE__)
#define RSP_HEADER_SET(key, value) response_header_set(response, key, value, __FUNCTION__, __LINE__)

#define PARAMS(key)         params_get(request, key)
#define PARAMS_COUNT        params_count(request)
#define PARAM_KEY(index)    param_key(request, index)
#define PARAM_VALUE(index)  param_value(request, index)

#endif
