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
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "dino_buffer.h"
#include "dino.h"
#include "dino_http.h"
#include "dino_utils.h"

#define DINO_VERSION "Server: dinohttpd/0.1\r\n"

dino_route *list_method_find(dino_route *list, http_method method, stack_char_ptr *url_stack)
{
    bool match = false;
    
    while(NULL != list)
    {
        int index = 0;
        
        if(method == list->method)
        {
            // Do they have the same number of elements?
            //
            if (list->stack->count == url_stack->count)
            {
                // Assume that we have a match...
                //
                match = true;
                
                // Now comapre the elements.
                //
                for (; index < list->stack->count && index < url_stack->count; index++)
                {
                    // If it is a wild card then assume it is good!
                    //
                    if (list->stack->ptrs[index][0] == '*')
                    {
                        continue;
                    }
                    
                    // Do we have a :param element?
                    //
                    if (list->stack->ptrs[index][0] == ':')
                    {
                        continue;
                    }
                    
                    // Ok see if they are the same string
                    //
                    if (0 != strcmp(list->stack->ptrs[index], url_stack->ptrs[index]))
                    {
                        match = false;
                        break;
                    }
                }
            }
        }
        
        if (match)
        {
            break;
        }
        
        list = list->next;
    }

    return list;
}

//
// Errors
//

void send_free(int client, char **buffer, int bytes)
{
    if (NULL != buffer)
    {
        if (bytes > 0)
        {
            send(client, *buffer, bytes, 0);
        }
        
        if (*buffer)
        {
            free(*buffer);
            *buffer = NULL;
        }
    }
}

void bad_request(int client)
{
    char *buffer = NULL;
    
    int bytes = asprintf(&buffer, "HTTP/1.0 400 BAD REQUEST\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "Content-type: text/html\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "<P>Your browser sent a bad request, ");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "such as a POST without a Content-Length.\r\n");
    send_free(client, &buffer, bytes);;
}

void cannot_execute(int client)
{
    char *buffer = NULL;
    
    int bytes = asprintf(&buffer, "HTTP/1.0 500 Internal Server Error\r\n");
    send_free(client, &buffer, bytes);;

    bytes = asprintf(&buffer, "Content-type: text/html\r\n");
    send_free(client, &buffer, bytes);;

    bytes = asprintf(&buffer, "\r\n");
    send_free(client, &buffer, bytes);;

    bytes = asprintf(&buffer, "<P>Error prohibited CGI execution.\r\n");
    send_free(client, &buffer, bytes);;
}

void not_found(int client)
{
    char *buffer = NULL;
    
    int bytes = asprintf(&buffer, "HTTP/1.0 404 NOT FOUND\r\n");
    send_free(client, &buffer, bytes);;

    bytes = asprintf(&buffer, DINO_VERSION);
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "Content-Type: text/html\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "<BODY><P>The server could not fulfill\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "your request because the resource specified\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "is unavailable or nonexistent.\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "</BODY></HTML>\r\n");
    send_free(client, &buffer, bytes);;
}

void unimplemented(int client)
{
    char *buffer = NULL;
    
    int bytes = asprintf(&buffer, "HTTP/1.0 501 Method Not Implemented\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, DINO_VERSION);
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "Content-Type: text/html\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "</TITLE></HEAD>\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "<BODY><P>HTTP request method not supported.\r\n");
    send_free(client, &buffer, bytes);;
    
    bytes = asprintf(&buffer, "</BODY></HTML>\r\n");
    send_free(client, &buffer, bytes);;
}

void log_error(const char *sc)
{
    perror(sc);
}

//
// Start up connection
//

int startup_connection(dino_site *psite)
{
    int httpd = 0;
    struct sockaddr_in name;

    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
    {
        log_error("socket");
    }
    
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(psite->port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
        log_error("bind");
    }
    
    // Dynamically allocate the port?
    //
    if (0 == psite->port)
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
        {
            log_error("getsockname");
        }
        
        psite->port = ntohs(name.sin_port);
    }
    
    if (listen(httpd, 5) < 0)
    {
        log_error("listen");
    }
    return(httpd);
}

//
// Read data from socket
//

size_t read_line(int socket, BUFFER *buffer)
{
    if ( NULL == buffer)
    {
        return 0;
    }
    
    // Free the buffer because the expectation is that we are reading in a
    // whole new line.
    //
    buffer_free(*buffer);
    *buffer = NULL;
    
    // Now we read in the data:
    //
    char c = '\0';
    
    while (c != '\n')
    {
        ssize_t n = recv(socket, &c, 1, 0);
        
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(socket, &c, 1, MSG_PEEK);

                if ((n > 0) && (c == '\n'))
                {
                    recv(socket, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }
            *buffer = buffer_append_char(*buffer, c);
        }
        else
        {
            c = '\n';
        }
    }
    
    if ( buffer_data_size(*buffer))
    {
        c = '\0';
        *buffer = buffer_append_char(*buffer, '\0');
    }
    
    return buffer_data_size(*buffer);
}

size_t read_data(int socket, char *data, size_t size)
{
    clear_memory(data, size);
    
    size_t i = 0;
    char c = '\0';
    
    while (i < size)
    {
        ssize_t n = recv(socket, &c, 1, 0);
        
        if (n > 0)
        {
            data[i] = c;
            ++i;
        }
    }
    
    return i;
}

//
// Parsers
//

http_method map_string_to_http_method(const char* method)
{
    http_method result = http_invalid;
    
    if (0 == strcasecmp(method, "GET"))
    {
        result = http_get;
    }
    else if (0 == strcasecmp(method, "POST"))
    {
        result = http_post;
    }
    else if (0 == strcasecmp(method, "PUT"))
    {
        result = http_put;
    }
    else if (0 == strcasecmp(method, "DELETE"))
    {
        result = http_delete;
    }
    else if (0 == strcasecmp(method, "OPTIONS"))
    {
        result = http_options;
    }
    else if (0 == strcasecmp(method, "HEAD"))
    {
        result = http_head;
    }
    else if (0 == strcasecmp(method, "TRACE"))
    {
        result = http_trace;
    }
    else if (0 == strcasecmp(method, "CONNECT"))
    {
        result = http_connect;
    }
   
    return result;
}


http_method parse_method_url(http_data *http)
{
    // Should not happen:
    //
    if ( NULL == http )
    {
        return http_invalid;
    }

    // Read in the data:
    //
    BUFFER buffer = NULL;
    
    if (0 == read_line(http->socket, &buffer))
    {
        buffer_free(buffer);
        return http_invalid;
    }

    // Obtain the method
    //
    char method[32];
    clear_memory(method, sizeof(method));
    int offset = 0;

    const char *line = buffer_data_ptr(buffer);
    
    for (int i = 0; !isspace(line[i]) && (i < sizeof(method)); i++)
    {
        method[i] = line[i];
        offset++;
    }

    http->request.method = map_string_to_http_method(method);
    
    if (http_invalid == http->request.method )
    {
        buffer_free(buffer);
        return http_invalid;
    }

    // Fetch the URL
    //
    const char *query = line + offset;
    BUFFER buffer_url = NULL;
    
    // Skip over the ' 's and the tabs
    //
    while(*query != '\0' && (*query == ' ' || *query == '\t'))
    {
        ++query;
    }
    
    while ( *query != '\0' && *query != '?' && *query != ' ' && *query != '\t')
    {
        buffer_append_char(buffer_url, *query);
        query++;
    }
    
    // Allocate and copy url to buffer
    //
    http->request.url = malloc_and_clear(buffer_data_size(buffer_url) + 1);
    strncpy(http->request.url, buffer_data_ptr(buffer_url), buffer_data_size(buffer_url));

    // Free the URL Buffer
    //
    buffer_free(buffer_url);
    buffer_url = NULL;
    
    if (*query == '?')
    {
        // Move off of '?'
        //
        query++;
        
        offset = 0;
        
        BUFFER buffer_params = NULL;
        
        while( !isspace(*query) && *query != 0 )
        {
            buffer_params = buffer_append_char(buffer_params, *query);
            offset++;
            query++;
        }

        buffer_params = buffer_append_char(buffer_params, '\0');

        // Parse the URL parameters
        //
        char *brkt = NULL;

        for (char *param = strtok_r(buffer_data_ptr(buffer_params), "&", &brkt);
             param;
             param = strtok_r(NULL, "&", &brkt))
        {
            char *brkt2 = NULL;
            char *key = strtok_r(param, "=", &brkt2);
            char *value = strtok_r(NULL, "=", &brkt2);

            if (key && *key && http->request.param_index < HTTP_MAX_PARAMS)
            {
                strncpy(http->request.params[http->request.param_index].key,
                        key,
                        sizeof(http->request.params[http->request.param_index].key));
                if (value && *value)
                {
                    strncpy(http->request.params[http->request.param_index].value,
                            value,
                            sizeof(http->request.params[http->request.param_index].value));
                }
                http->request.param_index++;
            }
        }
        
        buffer_free(buffer_params);
    }
    buffer_free(buffer);
    return http->request.method;
}

char *clean_string(char *value)
{
    char *zap =strrchr(value, '\n');
    if (zap) *zap = 0;
    zap = strrchr(value, '\r');
    if (zap) *zap = 0;
    
    if (*value == ' ')
    {
        value++;        
    }
    
    return value;
}

size_t parse_headers(http_data *http)
{
    size_t content_size = 0;
    size_t line_len = 0;
    
    // Read the headers...
    //

    BUFFER buffer = NULL;
    while ((line_len = read_line(http->socket, &buffer)))
    {
        // Find the key and the value
        //
        char *key = buffer_data_ptr(buffer);
        char *value = buffer_data_ptr(buffer);
        
        // Look for the ':'
        //
        for (; *value && *value != ':'; value++){};
        
        // Get the "Value
        //
        if (*value == ':')
        {
            *value = 0;
            value++;
        }
        
        key = clean_string(key);
        value = clean_string(value);
        
        // Copy to parameter list
        //
        if (key && *key && http->request.param_index < HTTP_MAX_PARAMS)
        {
            strncpy(http->request.params[http->request.param_index].key,
                    key,
                    sizeof(http->request.params[http->request.param_index].key));
            if (value && *value)
            {
                // Copy over the "value"
                //
                strncpy(http->request.params[http->request.param_index].value,
                        value,
                        sizeof(http->request.params[http->request.param_index].value));
            }
            http->request.param_index++;
        }
        else if (http->request.param_index >= HTTP_MAX_PARAMS)
        {
            fprintf(stderr, "WARNING: More than %d parameters found, ignorning: %s\n\r",
                    HTTP_MAX_PARAMS,
                    key);
        }
        
        // Check to see if we have hit the end...
        //
        if (key && *key == '\0')
        {
            // We hit the end!
            //
            break;
        }
        
        // OK look of content length
        //
        if (0 == strncmp(key, "Content-Length", HTTP_MAX_KEY_SIZE))
        {
            content_size = atol(value);
        }
    }
    
    buffer_free(buffer);
    return content_size;
}

//
// Read in the request
//

bool read_request(http_data *http)
{
    // Validate the Method:
    //
    if (http_invalid == parse_method_url(http))
    {
        return false;
    }
    
    // Read the headers...
    //
    http->request.content_size = parse_headers(http);
    
    // Get the content
    //
    if (http->request.content_size)
    {
        http->request.data = malloc_and_clear(http->request.content_size);
        size_t total = read_data(http->socket, http->request.data, http->request.content_size);
        assert(total == http->request.content_size);
        http->request.content_size = total;
    }
    
    return true;
}

bool bind_url_params(http_request *request, dino_route *route, stack_char_ptr *url_stack)
{
    // Now comapre the elements.
    //
    for (int index = 0; index < route->stack->count && index < url_stack->count; index++)
    {
        // If it is a wild card then Just skip it.
        //
        if (route->stack->ptrs[index][0] == '*')
        {
            continue;
        }
        
        // Do we have a :param element?
        //
        if (route->stack->ptrs[index][0] == ':')
        {
            if (request->param_index < HTTP_MAX_PARAMS)
            {
                strncpy(request->params[request->param_index].key,
                        route->stack->ptrs[index],
                        sizeof(request->params[request->param_index].key));
                if (url_stack->ptrs[index] && *url_stack->ptrs[index])
                {
                    strncpy(request->params[request->param_index].value,
                            url_stack->ptrs[index],
                            sizeof(request->params[request->param_index].value));
                }
                request->param_index++;
            }

            continue;
        }        
    }
    
    return true;
}

void invoke_method(dino_route *route, http_data *http, stack_char_ptr *url_stack)
{
    if (NULL == http || NULL == route || NULL == url_stack)
    {
        log_error("invoke_method: Invlaid inputs... NULL");
        return;
    }
    
    // bind url parameters
    //
    bind_url_params(&http->request, route, url_stack);
    
    // Invoke the method..
    //
    int http_error_code = route->verb_func(http);
    
    // Output Response Headers
    //
    char *buffer = NULL;
    
    int bytes = asprintf(&buffer, "HTTP/1.0 %d\r\n", http_error_code);
    send_free(http->socket, &buffer, bytes);
    
    
    for (int index = 0; index < http->response.param_index; index++)
    {
        bytes = asprintf(&buffer, "%s: %s\r\n",
                         http->response.params[index].key,
                         http->response.params[index].value);
        send_free(http->socket, &buffer, bytes);
    }
    
    bytes = asprintf(&buffer, "\r\n");
    send_free(http->socket, &buffer, bytes);
    
    // Output the data payload
    //
    send(http->socket, buffer_data_ptr(http->response.buffer_handle), buffer_data_size(http->response.buffer_handle), 0);
    
    // Free the buffer
    //
    buffer_free(http->response.buffer_handle);
}

void free_request(dino_handle *dhandle)
{
    if ( NULL != dhandle)
    {
        if ( dhandle->http.request.url)
        {
            free(dhandle->http.request.url);
            dhandle->http.request.url = NULL;
        }
    }
}

void accept_request(dino_site *psite, int socket)
{
    dino_handle dhandle;
    clear_memory(&dhandle, sizeof(dhandle));
    
    dhandle.http.handle_type = dino_handle_http;
    dhandle.http.socket = socket;

    if(!read_request(&dhandle.http))
    {
        free_request(&dhandle);
        bad_request(socket);
        return;
    }
    
    // Parse the URL Parameters.
    //
    stack_char_ptr *url_stack = stack_ptr_parse(NULL, dhandle.http.request.url, "/");

    // Search for a match...
    //
    dino_route *route = list_method_find(psite->list, dhandle.http.request.method, url_stack);
    
    // Do we have a route?
    //
    if (NULL != route)
    {
        invoke_method(route, &dhandle.http, url_stack);
    }
    else
    {
        fprintf(stderr, "ERROR: Path %s not found\n\r", dhandle.http.request.url);
    }

    stack_ptr_free(url_stack);
    free_request(&dhandle);

    close(socket);
}

int g_server_socket = 0;

void dino_start_http(dino_site *psite)
{
    if (NULL == psite)
    {
        fprintf(stderr, "ERROR: The site is not defined..\n\r");
        return;
    }
    
    struct sockaddr_in sockaddr_client;
    socklen_t sockaddr_client_length = sizeof(sockaddr_client);
    clear_memory(&sockaddr_client, sockaddr_client_length);
    
    g_server_socket = startup_connection(psite);
    
    fprintf(stdout, "Dino has taking the stage on port %d\n\r", psite->port);
    
    while (g_dino_keep_running)
    {
        int client_socket = accept(g_server_socket, (struct sockaddr *)&sockaddr_client, &sockaddr_client_length);
        if (client_socket == -1)
        {
            log_error("accept");
        }
        
        accept_request(psite, client_socket);
    }
    
    close(g_server_socket);
}

void dino_stop_http()
{
    close(g_server_socket);
}