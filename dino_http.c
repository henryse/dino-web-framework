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
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include "dino_buffer.h"
#include "dino.h"
#include "dino_http.h"
#include "dino_utils.h"

#define DINO_VERSION "Server: dinohttpd/0.1\r\n"

dino_route* list_create(dino_route **head)
{
    if(NULL == head)
    {
        return NULL;
    }
    
    dino_route *ptr = (dino_route*)malloc_and_clear(sizeof(dino_route));
    if(NULL != ptr)
    {
        *head = ptr;
    }
    
    return ptr;
}

dino_route* list_add_new_item(dino_route **head)
{
    if(NULL == head)
    {
        return NULL;
    }
    
    if(NULL == *head)
    {
        return (list_create(head));
    }
    
    dino_route *ptr = (dino_route*)malloc_and_clear(sizeof(dino_route));
    
    if(NULL == ptr)
    {
        return NULL;
    }
    
    ptr->next = *head;
    *head = ptr;
    
    return ptr;
}

dino_route *list_find(dino_route *list, const char *name)
{
    while(NULL != list && strncmp(name, list->name, strlen(list->name)))
    {
        list = list->next;
    }
    
    return list;
}

dino_route *list_method_find(dino_route *list, http_method method, const char *url)
{
    stack_char_ptr *url_stack = stack_ptr_parse(NULL, url, "/");
    
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
    
    stack_ptr_free(url_stack);
    
    return list;
}

//
// Errors
//

void bad_request(int client)
{
    char buf[MAX_ERROR_LENGTH];
    
    int bytes = snprintf(buf, sizeof(buf), "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, bytes, 0);

    bytes = snprintf(buf, sizeof(buf), "Content-type: text/html\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "<P>Your browser sent a bad request, ");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "such as a POST without a Content-Length.\r\n");
    send(client, buf, bytes, 0);
}

void cannot_execute(int client)
{
    char buf[MAX_ERROR_LENGTH];
    
    int bytes = snprintf(buf, sizeof(buf), "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, bytes, 0);

    bytes = snprintf(buf, sizeof(buf), "Content-type: text/html\r\n");
    send(client, buf, bytes, 0);

    bytes = snprintf(buf, sizeof(buf), "\r\n");
    send(client, buf, bytes, 0);

    bytes = snprintf(buf, sizeof(buf), "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, bytes, 0);
}

void not_found(int client)
{
    char buf[MAX_ERROR_LENGTH];
    
    int bytes = snprintf(buf, sizeof(buf), "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, bytes, 0);

    bytes = snprintf(buf, sizeof(buf), DINO_VERSION);
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "Content-Type: text/html\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "your request because the resource specified\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "is unavailable or nonexistent.\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "</BODY></HTML>\r\n");
    send(client, buf, bytes, 0);
}

void unimplemented(int client)
{
    char buf[MAX_ERROR_LENGTH];
    
    int bytes = snprintf(buf, sizeof(buf), "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), DINO_VERSION);
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "Content-Type: text/html\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "</TITLE></HEAD>\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, bytes, 0);
    
    bytes = snprintf(buf, sizeof(buf), "</BODY></HTML>\r\n");
    send(client, buf, bytes, 0);
}

void throw_error(const char *sc)
{
    perror(sc);
    exit(1);
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
        throw_error("socket");
    }
    
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(psite->port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
        throw_error("bind");
    }
    
    // Dynamically allocate the port?
    //
    if (0 == psite->port)
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
        {
            throw_error("getsockname");
        }
        
        psite->port = ntohs(name.sin_port);
    }
    
    if (listen(httpd, 5) < 0)
    {
        throw_error("listen");
    }
    return(httpd);
}

//
// Read data from socket
//

size_t read_line(http_request *request, char *line, size_t size)
{
    clear_memory(line, size);
    
    size_t i = 0;
    char c = '\0';
    
    while ((i < size) && (c != '\n'))
    {
        ssize_t n = recv(request->socket, &c, 1, 0);
        
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(request->socket, &c, 1, MSG_PEEK);

                if ((n > 0) && (c == '\n'))
                {
                    recv(request->socket, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }
            line[i] = c;
            ++i;
        }
        else
        {
            c = '\n';
        }
    }
    
    return i;
}

size_t read_data(http_request *request, char *data, size_t size)
{
    clear_memory(data, size);
    
    size_t i = 0;
    char c = '\0';
    
    while (i < size)
    {
        ssize_t n = recv(request->socket, &c, 1, 0);
        
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

http_method parse_method_url(http_request *request, char *line, size_t size)
{
    if (request == NULL || line == NULL)
    {
        return http_invalid;
    }

    // Obtain the method
    //
    char method[32];
    clear_memory(method, sizeof(method));
    int offset = 0;

    for (int i = 0; !isspace(line[i]) && (i < sizeof(method)); i++)
    {
        method[i] = line[i];
        offset++;
    }

    request->method = http_invalid;
    
    if (0 == strcasecmp(method, "GET"))
    {
        request->method = http_get;
    }
    else if (0 == strcasecmp(method, "POST"))
    {
        request->method = http_post;
    }
    else if (0 == strcasecmp(method, "PUT"))
    {
        request->method = http_put;
    }
    else if (0 == strcasecmp(method, "DELETE"))
    {
        request->method = http_delete;
    }
    else if (0 == strcasecmp(method, "OPTIONS"))
    {
        request->method = http_options;
    }
    else if (0 == strcasecmp(method, "HEAD"))
    {
        request->method = http_head;
    }
    else if (0 == strcasecmp(method, "TRACE"))
    {
        request->method = http_trace;
    }
    else if (0 == strcasecmp(method, "CONNECT"))
    {
        request->method = http_connect;
    }
    else
    {
        return http_invalid;
    }

    // Fetch the URL
    //
    char *query = line + offset;
    char *url = request->url;
    
    // Skip over the ' 's and the tabs
    //
    while(*query != '\0' && (*query == ' ' || *query == '\t'))
    {
        ++query;
    }
    
    while ( *query != '\0' && *query != '?' && *query != ' ' && *query != '\t')
    {
        *url = *query;
        query++;
        url++;
    }
    
    if (*query == '?')
    {
        query++;
    }
    
    offset = 0;
    
    char params_buffer[HTTP_URL_SIZE];
    while(!isspace(*query) && *query != 0 && offset < sizeof(request->url) )
    {
        params_buffer[offset] = *query;
        offset++;
        query++;
    }

    // Parse the URL parameters
    //
    char *brkt = NULL;

    for (char *param = strtok_r(params_buffer, "&", &brkt);
         param;
         param = strtok_r(NULL, "&", &brkt))
    {
        char *brkt2 = NULL;
        char *key = strtok_r(param, "=", &brkt2);
        char *value = strtok_r(NULL, "=", &brkt2);

        if (key && *key && request->param_index < HTTP_MAX_PARAMS)
        {
            strncpy(request->params[request->param_index].key, key, sizeof(request->params[request->param_index].key));
            if (value && *value)
            {
                strncpy(request->params[request->param_index].value, value, sizeof(request->params[request->param_index].value));
            }
            request->param_index++;
        }
    }

    return request->method;
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

size_t parse_headers(http_request *request, char *line, size_t size)
{
    size_t content_size = 0;
    
    clear_memory(line, size);
    size_t line_len = 0;

    // Read the headers...
    //
    while ((line_len = read_line(request, line, size)))
    {
        // Find the key and the value
        //
        char *key = line;
        char *value = line;
        
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
        if (key && *key && request->param_index < HTTP_MAX_PARAMS)
        {
            strncpy(request->params[request->param_index].key, key, sizeof(request->params[request->param_index].key));
            if (value && *value)
            {
                // Copy over the "value"
                //
                strncpy(request->params[request->param_index].value, value, sizeof(request->params[request->param_index].value));
            }
            request->param_index++;
        }
        else if (request->param_index >= HTTP_MAX_PARAMS)
        {
            fprintf(stderr, "WARNING: More than %d parameters found, ignorning: %s\n\r", HTTP_MAX_PARAMS, key);
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
    
    return content_size;
}

//
// Read in the request
//

bool read_request(http_request *request)
{
    char line[MAX_HTTP_HEADER_LINE_LENGTH];
    clear_memory(line, sizeof(line));

    // Read the Method:
    //
    size_t line_len = read_line(request, line, sizeof(line));
    
    if (0 == line_len)
    {
        return false;
    }

    assert(strlen(line) == line_len);

    // Validate the Method:
    //
    if (http_invalid == parse_method_url(request, line, line_len))
    {
        return false;
    }
    
    // Read the headers...
    //
    request->content_size = parse_headers(request, line, sizeof(line));
    
    // Get the content
    //
    if (request->content_size)
    {
        request->data = malloc_and_clear(request->content_size);
        size_t total = read_data(request, request->data, request->content_size);
        assert(total == request->content_size);
        request->content_size = total;
    }
    
    return true;
}

bool bind_url_params(http_request *request, dino_route *route)
{
    stack_char_ptr *url_stack = stack_ptr_parse(NULL, request->url, "/");
    
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
            // TODO: push :name, value into params.
            continue;
        }        
    }
    
    stack_ptr_free(url_stack);
    
    return true;
}

void accept_request(dino_site *psite, int client)
{
    http_request request;
    clear_memory(&request, sizeof(request));
    
    request.socket = client;
    if(!read_request(&request))
    {
        bad_request(client);
        return;
    }
    
    // Search for a match...
    //
    dino_route *route = list_method_find(psite->list, request.method, request.url);
    
    if (NULL != route)
    {
        // bind url parameters
        //
        bind_url_params(&request, route);
        
        http_response response;
        clear_memory(&response, sizeof(response));
        
        response.client = client;

        // Invoke the method..
        //
        int http_error_code = route->verb_func(&request, &response);
        
        // Output Response
        //
        char buf[MAX_HTTP_HEADER_LINE_LENGTH];
        clear_memory(&buf, sizeof(buf));
        
        int bytes = snprintf(buf, sizeof(buf), "HTTP/1.0 %d\r\n", http_error_code);
        send(client, buf, bytes, 0);
        
        for (int index = 0; index < response.param_index; index++)
        {
            clear_memory(&buf, sizeof(buf));
            bytes = snprintf(buf, sizeof(buf), "%s: %s\r\n", response.params[index].key, response.params[index].value);
            send(client, buf, bytes, 0);
        }
        
        clear_memory(&buf, sizeof(buf));
        bytes = snprintf(buf, sizeof(buf), "\r\n");
        send(client, buf, bytes, 0);

        send(client, buffer_data(response.buffer_handle), buffer_size(response.buffer_handle), 0);
    }
    else
    {
        fprintf(stderr, "ERROR: Path %s not found\n\r", request.url);
    }

    close(client);
}

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
    
    int server_socket = startup_connection(psite);
    
    fprintf(stdout, "Dino has taking the stage on port %d\n\r", psite->port);
    
    while (1)
    {
        int client_socket = accept(server_socket, (struct sockaddr *)&sockaddr_client, &sockaddr_client_length);
        if (client_socket == -1)
        {
            throw_error("accept");
        }
        
        accept_request(psite, client_socket);
    }
    
    close(server_socket);
}