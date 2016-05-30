/**********************************************************************
//    Copyright (c) 2015 Henry Seurer
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
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#define _GNU_SOURCE

#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "dino.h"
#include "dino_http.h"

#define DINO_VERSION "Server: dinohttp/0.1\r\n"

dino_route_t *list_method_find(dino_route_t *list, dino_http_method method, stack_char_ptr_t *url_stack) {
    bool match = false;

    while (NULL != list) {
        int index = 0;

        if (method == list->method) {
            // Do they have the same number of elements?
            //
            if (list->stack->count == url_stack->count) {
                // Assume that we have a match...
                //
                match = true;

                // Now compare the elements.
                //
                for (; index < list->stack->count && index < url_stack->count; index++) {
                    // If it is a wild card then assume it is good!
                    //
                    if (list->stack->pointers[index][0] == '*') {
                        continue;
                    }

                    // Do we have a :param element?
                    //
                    if (list->stack->pointers[index][0] == ':') {
                        continue;
                    }

                    // Ok see if they are the same string
                    //
                    if (0 != strcmp(list->stack->pointers[index], url_stack->pointers[index])) {
                        match = false;
                        break;
                    }
                }
            }
        }

        if (match) {
            break;
        }

        list = list->next;
    }

    return list;
}

//
// Errors
//

void send_free(int client, char **buffer, int bytes) {
    if (NULL != buffer) {
        if (bytes > 0) {
            send(client, *buffer, (size_t) bytes, 0);
        }

        if (*buffer) {
            memory_free(*buffer);
            *buffer = NULL;
        }
    }
}

void bad_request(int client) {
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

void cannot_execute(int client) {
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

void not_found(int client) {
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

void unimplemented(int client) {
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

void log_error(const char *sc, const char *function, int line) {
    perror(sc);
    fprintf(stderr, "[SYSTEM ERROR][%s:%d]\n\r", function, line);
}

//
// Start up connection
//

int startup_connection(dino_http_site_t *dino_site) {
    int sockfd = 0;
    struct sockaddr_in name;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_error("socket", __FUNCTION__, __LINE__);
    }
    else {
        memset(&name, 0, sizeof(name));
        name.sin_family = AF_INET;
        name.sin_port = htons(dino_site->port);
        name.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sockfd, (struct sockaddr *) &name, sizeof(name)) < 0) {
            log_error("bind", __FUNCTION__, __LINE__);
            close(sockfd);
            sockfd = -1;
        }
        else {
            if (0 == dino_site->port) {
                socklen_t namelen = sizeof(name);
                if (getsockname(sockfd, (struct sockaddr *) &name, &namelen) == -1) {
                    log_error("getsockname", __FUNCTION__, __LINE__);
                    close(sockfd);
                    sockfd = -1;
                }
                else {
                    dino_site->port = ntohs(name.sin_port);
                }
            }

            if (sockfd != -1) {
                if (listen(sockfd, 5) < 0) {
                    log_error("listen", __FUNCTION__, __LINE__);
                    close(sockfd);
                    sockfd = -1;
                }
            }
        }
    }

    return (sockfd);
}

//
// Read data from socket
//

size_t read_line(int socket, STRING_BUFFER_PTR *string_buffer_ptr) {

    // If input is invalid then just error out.
    //
    if (NULL == string_buffer_ptr){
        return 0;
    }

    // We are gettign a new "line" so wack the old one if it exists.
    //
    if (*string_buffer_ptr){
        string_buffer_reset(*string_buffer_ptr);
    }
    else{
        *string_buffer_ptr = string_buffer_new();
    }

    // Now we read in the data:
    //
    char c = '\0';

    while (c != '\n') {
        ssize_t n = recv(socket, &c, 1, 0);

        if (n > 0) {
            if (c == '\r') {
                n = recv(socket, &c, 1, MSG_PEEK);

                if ((n > 0) && (c == '\n')) {
                    recv(socket, &c, 1, 0);
                }
                else {
                    c = '\n';
                }
            }

            string_buffer_append_char(*string_buffer_ptr, c);
        }
        else {
            c = '\n';
        }
    }

    return string_buffer_c_string_length(*string_buffer_ptr);
}

size_t read_data(int socket, char *data, size_t size) {
    memory_clear(data, size);

    size_t i = 0;
    char c = '\0';

    while (i < size) {
        ssize_t n = recv(socket, &c, 1, 0);

        if (n > 0) {
            data[i] = c;
            ++i;
        }
    }

    return i;
}

//
// Parsers
//

dino_http_method map_string_to_http_method(const char *method) {
    dino_http_method result = http_invalid;

    if (0 == strcasecmp(method, "GET")) {
        result = http_get;
    }
    else if (0 == strcasecmp(method, "POST")) {
        result = http_post;
    }
    else if (0 == strcasecmp(method, "PUT")) {
        result = http_put;
    }
    else if (0 == strcasecmp(method, "DELETE")) {
        result = http_delete;
    }
    else if (0 == strcasecmp(method, "OPTIONS")) {
        result = http_options;
    }
    else if (0 == strcasecmp(method, "HEAD")) {
        result = http_head;
    }
    else if (0 == strcasecmp(method, "TRACE")) {
        result = http_trace;
    }
    else if (0 == strcasecmp(method, "CONNECT")) {
        result = http_connect;
    }

    return result;
}


dino_http_method parse_method_url(dino_http_data_t *http) {
    // Should not happen:
    //
    if (NULL == http) {
        return http_invalid;
    }

    // Read in the data:
    //
    STRING_BUFFER_PTR string_buffer_ptr = NULL;

    if (0 == read_line(http->socket, &string_buffer_ptr)) {
        string_buffer_delete(string_buffer_ptr, true);
        return http_invalid;
    }

    // Obtain the method
    //
    char method[32];
    memory_clear(method, sizeof(method));
    int offset = 0;

    const char *line = string_buffer_c_string(string_buffer_ptr);

    for (int i = 0; !isspace(line[i]) && (i < sizeof(method)); i++) {
        method[i] = line[i];
        offset++;
    }

    http->request.method = map_string_to_http_method(method);

    if (http_invalid == http->request.method) {
        string_buffer_delete(string_buffer_ptr, true);
        return http_invalid;
    }

    // Fetch the URL
    //
    const char *query = line + offset;
    STRING_BUFFER_PTR buffer_url = string_buffer_new();

    // Skip over the ' 's and the tabs
    //
    while (*query != '\0' && (*query == ' ' || *query == '\t')) {
        ++query;
    }

    while (*query != '\0' && *query != '?' && *query != ' ' && *query != '\t') {
        string_buffer_append_char(buffer_url, *query);
        query++;
    }

    http->request.url = buffer_url;

    if (*query == '?') {
        // Move off of '?'
        //
        query++;

        offset = 0;

        STRING_BUFFER_PTR buffer_params = string_buffer_new();

        while (!isspace(*query) && *query != 0) {
            string_buffer_append_char(buffer_params, *query);
            offset++;
            query++;
        }

        // Parse the URL parameters
        //
        char *break_token = NULL;

        for (char *param = strtok_r(string_buffer_c_string(buffer_params), "&", &break_token);
             param;
             param = strtok_r(NULL, "&", &break_token))
        {
            char *break_token_2 = NULL;
            char *key = strtok_r(param, "=", &break_token_2);
            char *value = strtok_r(NULL, "=", &break_token_2);

            dino_strmap_add(http->request.params_map, key, value);
        }

        string_buffer_delete(buffer_params, true);
    }

    string_buffer_delete(string_buffer_ptr, true);
    return http->request.method;
}

char *clean_string(char *value) {
    char *zap = strrchr(value, '\n');
    if (zap) *zap = 0;
    zap = strrchr(value, '\r');
    if (zap) *zap = 0;

    if (*value == ' ') {
        value++;
    }

    return value;
}

size_t parse_headers(dino_http_data_t *http) {
    size_t content_size = 0;

    // Read the headers...
    //
    STRING_BUFFER_PTR string_buffer_ptr = NULL;

    while (read_line(http->socket, &string_buffer_ptr)) {
        // Find the key and the value
        //
        char *key = string_buffer_c_string(string_buffer_ptr);
        char *value = string_buffer_c_string(string_buffer_ptr);

        // Look for the ':'
        //
        for (; *value && *value != ':'; value++) { };

        // Get the "Value
        //
        if (*value == ':') {
            *value = 0;
            value++;
        }

        key = clean_string(key);
        value = clean_string(value);

        if (!dino_strmap_add(http->request.params_map, key, value)) {
            fprintf(stderr, "[WARNING] Unable to add to params list...\n\r");
        }

        // Check to see if we have hit the end...
        //
        if (key && *key == '\0') {
            // We hit the end!
            //
            break;
        }

        // OK look of content length
        //
        if (0 == strncmp(key, "Content-Length", 32)) {
            content_size = (size_t) atol(value);
        }
    }

    string_buffer_delete(string_buffer_ptr, true);
    return content_size;
}

//
// Read in the request
//

bool read_request(dino_http_data_t *http) {
    // Validate the Method:
    //
    if (http_invalid == parse_method_url(http)) {
        return false;
    }

    // Read the headers...
    //
    http->request.content_size = parse_headers(http);

    // Get the content
    //
    if (http->request.content_size) {
        http->request.data = memory_alloc(http->request.content_size);
        size_t total = read_data(http->socket, http->request.data, http->request.content_size);
        assert(total == http->request.content_size);
        http->request.content_size = total;
    }

    return true;
}

bool bind_url_params(dino_http_request_t *request, dino_route_t *route, stack_char_ptr_t *url_stack) {
    // Now comapre the elements.
    //
    for (int index = 0; index < route->stack->count && index < url_stack->count; index++) {
        // If it is a wild card then Just skip it.
        //
        if (route->stack->pointers[index][0] == '*') {
            continue;
        }

        // Do we have a :param element?
        //
        if (route->stack->pointers[index][0] == ':') {
            dino_strmap_add(request->params_map, route->stack->pointers[index], url_stack->pointers[index]);
            continue;
        }
    }

    return true;
}

bool param_output(const char *key, const char *value, const void *obj) {
    dino_http_data_t *http = (dino_http_data_t *) obj;

    char *buffer = NULL;

    int bytes = asprintf(&buffer, "%s: %s\r\n", key, value);

    send_free(http->socket, &buffer, bytes);

    return true;
}

void invoke_method(dino_route_t *route, dino_http_data_t *http, stack_char_ptr_t *url_stack) {
    if (NULL == http || NULL == route || NULL == url_stack) {
        log_error("invoke_method: Invlaid inputs... NULL", __FUNCTION__, __LINE__);
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

    dino_strmap_enum(http->response.params_map, param_output, http);

    bytes = asprintf(&buffer, "\r\n");
    send_free(http->socket, &buffer, bytes);

    // Output the data payload
    //
    send(http->socket, string_buffer_c_string(http->response.buffer_handle),
         string_buffer_c_string_length(http->response.buffer_handle),
         0);
}

void init_request(dino_handle_t *dhandle) {
    memory_clear(dhandle, sizeof(dino_handle_t));
    dhandle->http.handle_type = dino_handle_http;

    dhandle->http.request.params_map = dino_strmap_new(32);
    dhandle->http.response.params_map = dino_strmap_new(32);
}

void free_request(dino_handle_t *dhandle) {
    if (NULL != dhandle) {
        if (dhandle->http.request.url) {
            memory_free(dhandle->http.request.url);
            dhandle->http.request.url = NULL;
        }

        string_buffer_delete(dhandle->http.response.buffer_handle, true);
        dhandle->http.response.buffer_handle = NULL;

        dino_strmap_delete(dhandle->http.request.params_map);
        dhandle->http.request.params_map = NULL;

        dino_strmap_delete(dhandle->http.response.params_map);
        dhandle->http.response.params_map = NULL;
    }
}

void accept_request(dino_http_site_t *dino_site, dino_handle_t *dhandle) {
    // Setup DHANDLE:
    //
    if (!read_request(&dhandle->http)) {
        bad_request(dhandle->http.socket);
    }
    else {
        // Parse the URL Parameters.
        //
        stack_char_ptr_t *url_stack = stack_ptr_parse(NULL, string_buffer_c_string(dhandle->http.request.url), "/");

        // Search for a match...
        //
        dino_route_t *route = list_method_find(dino_site->list, dhandle->http.request.method, url_stack);

        // Do we have a route?
        //
        if (NULL != route) {
            invoke_method(route, &dhandle->http, url_stack);
        }
        else {
            fprintf(stderr, "[ERROR] Path %s not found; \n\r", string_buffer_c_string(dhandle->http.request.url));
        }

        stack_ptr_free(url_stack);
    }
}

int g_server_socket = 0;

void dino_process_request(dino_http_site_t *dino_site, int socket) {
    dino_handle_t dhandle;
    init_request(&dhandle);

    if (-1 == socket) {
        log_error("accept", __FUNCTION__, __LINE__);
    }
    else {
        dhandle.http.socket = socket;
        accept_request(dino_site, &dhandle);
    }

    // Clear the cache memory...
    // This assumes that there is no memory allocations
    // that will be presisted across calls.
    //
    memory_cache_clear();

    // Free DHANDLE
    //
    free_request(&dhandle);

    // Close Socket
    //
    close(socket);
}

void dino_http_start(dino_http_site_t *dino_site) {
    if (NULL == dino_site) {
        fprintf(stderr, "[ERROR] The site is not defined..\n\r");
        return;
    }

    struct sockaddr_in sockaddr_client;
    socklen_t sockaddr_client_length = sizeof(sockaddr_client);
    memory_clear(&sockaddr_client, sockaddr_client_length);

    memory_cache_alloc(1024 * 16);

    g_server_socket = startup_connection(dino_site);

    if (-1 != g_server_socket) {
        fprintf(stdout, "[INFO] Dino has taking the stage on port %d\n\r", dino_site->port);

        while (g_dino_keep_running) {
            int client_socket = accept(g_server_socket, (struct sockaddr *) &sockaddr_client, &sockaddr_client_length);

            dino_process_request(dino_site, client_socket);
        }

        close(g_server_socket);
    }

    memory_cache_free();
}

void dino_http_stop() {
    int socket = g_server_socket;
    g_server_socket = -1;

    close(socket);

    sleep(2);
}

#pragma clang diagnostic pop