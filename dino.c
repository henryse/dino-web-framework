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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <signal.h>

#include "dino_buffer.h"
#include "dino.h"
#include "dino_http.h"
#include "dino_utils.h"

// DHANDLE Casting helpers functions.
//

dino_site *cast_dhandle_site(DHANDLE dhandle)
{
    if ( NULL != dhandle )
    {
        dino_site *ptr = (dino_site *)dhandle;
        
        if ( dino_handle_site == ptr->handle_type )
        {
            return ptr;
        }
    }
    return NULL;
}

http_request *cast_dhandle_request(DHANDLE dhandle)
{
    if ( NULL != dhandle )
    {
        http_data *ptr = (http_data *)dhandle;
        
        if ( dino_handle_http == ptr->handle_type )
        {
            return &ptr->request;
        }
    }
    return NULL;
}

http_response *cast_dhandle_response(DHANDLE dhandle)
{
    if ( NULL != dhandle )
    {
        http_data *ptr = (http_data *)dhandle;
        
        if ( dino_handle_http == ptr->handle_type )
        {
            return &ptr->response;
        }
    }
    return NULL;
}

// HTTP Verbs to function prefix strings...
//

const char* http_method_prefix_string(http_method method)
{
    switch(method)
    {
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

dino_route* list_add_new_item(dino_route **head)
{
    // List is invalid.
    //
    if(NULL == head)
    {
        return NULL;
    }

    // List is empty.
    //
    if (NULL == *head)
    {
        *head = (dino_route*)malloc_and_clear(sizeof(dino_route));
        return *head;
    }

    // Add to the end of the list.
    //
    dino_route *ptr = *head;

    while( ptr->next != NULL ) { ptr = ptr->next; }

    ptr->next = (dino_route*)malloc_and_clear(sizeof(dino_route));

    return ptr->next;
}

dino_route *list_find(dino_route *list, const char *name)
{
    while(NULL != list && strncmp(name, list->name, strlen(list->name)))
    {
        list = list->next;
    }
    
    return list;
}

bool add_method_to_site(http_method method, DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path)
{
    if(NULL == dhandle)
    {
        return false;
    }
    
    dino_site *psite = cast_dhandle_site(dhandle);
    
    if (NULL != list_find(psite->list, name))
    {
        return false;
    }
    
    dino_route *ppath = list_add_new_item(&psite->list);

    // Build name for method
    //
    const char *method_name = http_method_prefix_string(method);
    
    ppath->name = malloc_and_clear(strlen(method_name) + strlen(name) + 1);
    strncpy(ppath->name, method_name, strlen(method_name));
    strncat(ppath->name, name, strlen(name));
    
    // Store the path
    //
    ppath->path = malloc_and_clear(strlen(path) + 1);
    strncpy(ppath->path, path, strlen(path));
    
    // Parse the path, we need to see if there are any :[name] directives
    //
    ppath->stack = stack_ptr_parse(ppath->stack, ppath->path, "/");

    // Save callback function pointer
    //
    ppath->verb_func = verb_func;
    ppath->method = method;
    
    return true;
}

DHANDLE dino_config_start(unsigned int port,  char *host)
{
    dino_site *psite = (dino_site *)malloc_and_clear(sizeof(dino_site));
    if (NULL != psite)
    {
        psite->host = malloc_and_clear(strlen(host) + 1);
        strncpy(psite->host, host, strlen(host));
        
        psite->port = port;
        psite->list = NULL;
        psite->handle_type = dino_handle_site;
    }
    
    return psite;
}

bool dino_route_get(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_get, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] GET unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    
    return true;
}

bool dino_route_post(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_post, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] POST unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    
    return true;
}

bool dino_route_delete(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_delete, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] DELETE unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    return true;
}

bool dino_route_put(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_put, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] PUT unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    return true;
}

bool dino_route_options(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_options, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] OPTIONS unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    return true;
}

bool dino_route_head(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_head, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] HEAD unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    return true;
}

bool dino_route_trace(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_trace, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] TRACE unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    return true;
}

bool dino_route_connect(DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path, const char *function, int line)
{
    if (!add_method_to_site(http_connect, dhandle, verb_func, name, path))
    {
        fprintf(stderr, "ERROR:[%s:%d] CONNECT unable to bind %s to path %s\n\r", function, line, name, path );
        return false;
    }
    return true;
}

bool g_dino_keep_running = true;

void dino_signal_handler(int value)
{
    fprintf(stderr, "Shutting down the service, signal: %d\n\r", value );
    g_dino_keep_running = false;
    dino_stop_http();
}

bool dino_start(DHANDLE dhandle, const char *function, int line)
{
    if(NULL == dhandle)
    {
        fprintf(stderr, "ERROR:[%s:%d] Unable to start Dino, the dhandle is invlaid.\n\r", function, line );
        return false;
    }
    
    signal(SIGABRT, dino_signal_handler);
    signal(SIGFPE, dino_signal_handler);
    signal(SIGILL, dino_signal_handler);
    signal(SIGINT, dino_signal_handler);
    signal(SIGSEGV, dino_signal_handler);
    signal(SIGTERM, dino_signal_handler);
    
    dino_start_http(cast_dhandle_site(dhandle));

    return true;
}

void response_send(DHANDLE dhandle, const char *data, size_t size, const char *function, int line)
{
    http_response *response = cast_dhandle_response(dhandle);
    
    if (NULL != response )
    {
        response->buffer_handle = buffer_append_data(response->buffer_handle, data, size);
    }
    else
    {
        fprintf(stderr, "ERROR:[%s:%d] resposne_handle is NULL... \n\r", function, line);
    }
}

int response_printf(DHANDLE dhandle, const char *function, int line, const char *fmt, ...)
{
    // Points to each unnamed arg in turn
    //
    va_list ap;

    // make ap point to 1st unnamed arg
    //
    va_start(ap, fmt);

    // vasprintf with variable params
    //
    char *result = NULL;
    if (vasprintf(&result, fmt, ap) > 0)
    {
        if (NULL != result)
        {
            // now call the send method
            //
            response_send(dhandle, result, strlen(result), function, line);
        
            // Free the memory
            free(result);
        }
    }
    
    // clean up when done
    //
    va_end(ap);

    return 0;
}

void response_header_set(DHANDLE dhandle, const char *key, const char *value, const char *function, int line)
{
    http_response *response = cast_dhandle_response(dhandle);
    
    if ( NULL != response )
    {

        if (key && *key)
        {
            bool found_key = false;
            int index = 0;
            for (; index < response->param_index; index++)
            {
                if (0 == strncmp(response->params[index].key, key, sizeof(response->params[index].key)))
                {
                    found_key = true;
                    break;
                }
            }
            
            if (index < HTTP_MAX_PARAMS)
            {
                strncpy(response->params[index].key, key, sizeof(response->params[index].key));
                if (value && *value)
                {
                    strncpy(response->params[index].value, value, sizeof(response->params[index].value));
                }
                
                if (!found_key)
                {
                    response->param_index++;
                }
            }
        }
        else
        {
            fprintf(stderr, "ERROR:[%s:%d] HTTP_MAX_PARAMS exceeded... \n\r", function, line);
        }
    }
    else
    {
        fprintf(stderr, "ERROR:[%s:%d] resposne_handle is NULL... \n\r", function, line);
    }
}

const char *params_get(DHANDLE dhandle, const char *key)
{
    http_request *request = cast_dhandle_request(dhandle);

    if ( NULL != request )
    {
        for (int index = 0; index < request->param_index; index++)
        {
            if (0 == strncmp( request->params[index].key, key, sizeof( request->params[index].key)))
            {
                return request->params[index].value;
            }
        }
    }
    
    return "";
}

size_t params_count(DHANDLE dhandle)
{
    http_request *request = cast_dhandle_request(dhandle);
    if (NULL != request)
    {
        return request->param_index;
    }
    
    return 0;
}

const char *param_key(DHANDLE dhandle, size_t index)
{
    http_request *request = cast_dhandle_request(dhandle);
    if (NULL != request)
    {
        if (index < request->param_index)
        {
            return request->params[index].key;
        }
    }
    
    return "";
}

const char *param_value(DHANDLE dhandle, size_t index)
{
    http_request *request = cast_dhandle_request(dhandle);
    if (NULL != request)
    {        
        if (index < request->param_index)
        {
            return request->params[index].value;
        }
    }
    
    return "";
}
