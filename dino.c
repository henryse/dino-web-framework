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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "dino_buffer.h"
#include "dino.h"
#include "dino_http.h"
#include "dino_utils.h"

const char* method_name_get(http_method method)
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

bool add_method_to_site(http_method method, DHANDLE dhandle, http_verb_func verb_func, const char *name, const char *path)
{
    if(NULL == dhandle)
    {
        return false;
    }
    
    dino_site *psite = (dino_site *)dhandle;
    
    if (NULL != list_find(psite->list, name))
    {
        return false;
    }
    
    dino_route *ppath = list_add_new_item(&psite->list);

    // Build name for method
    //
    const char *method_name = method_name_get(method);
    
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
        strncpy(psite->host, host, sizeof(psite->host));
        psite->port = port;
        psite->list = NULL;
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

bool dino_start(DHANDLE dhandle, const char *function, int line)
{
    if(NULL == dhandle)
    {
        fprintf(stderr, "ERROR:[%s:%d] Unable to start Dino, the dhandle is invlaid.\n\r", function, line );
        return false;
    }
    
    dino_start_http((dino_site *)dhandle);

    return true;
}

bool dino_stop(DHANDLE dhandle, const char *function, int line)
{
    if(NULL == dhandle)
    {
        fprintf(stderr, "ERROR:[%s:%d] Unable to stop Dino... \n\r", function, line);
        return false;
    }
    
    return true;
}

void response_send(RESPONE response_handle, const char *data, size_t size, const char *function, int line)
{
    if (response_handle != NULL)
    {
        http_response *response = (http_response *)response_handle;
        
        response->buffer_handle = buffer_append(response->buffer_handle, data, size);
    }
    else
    {
        fprintf(stderr, "ERROR:[%s:%d] resposne_handle is NULL... \n\r", function, line);
    }
}

void response_header_set(RESPONE response_handle, const char *key, const char *value, const char *function, int line)
{
    if (response_handle != NULL)
    {
        http_response *response = (http_response *)response_handle;

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

const char *params_get(REQUEST request_handle, const char *key)
{
    if (NULL != request_handle)
    {
        http_request *request = (http_request *)request_handle;
        
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

size_t params_count(REQUEST request_handle)
{
    if (NULL != request_handle)
    {
        http_request *request = (http_request *)request_handle;
        return request->param_index;
    }
    
    return 0;
}

const char *param_key(REQUEST request_handle, size_t index)
{
    if (NULL != request_handle)
    {
        http_request *request = (http_request *)request_handle;
        
        if (index < request->param_index)
        {
            return request->params[index].key;
        }
    }
    
    return "";
}

const char *param_value(REQUEST request_handle, size_t index)
{
    if (NULL != request_handle)
    {
        http_request *request = (http_request *)request_handle;
        
        if (index < request->param_index)
        {
            return request->params[index].value;
        }
    }
    
    return "";
}
