#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "./bobcat.h"

#define BC_REQ_BASE_SIZE 64

struct bc_server_config
{
    int port;
    int socket_fd;
    int host_addrlen;
    struct sockaddr_in *host_addr;
    struct dictlist_node *handlers;
};

struct dispatch_args
{
    struct bc_server_config *config;
    int *p_fd;
};

char *bc_method_name(enum bc_request_method method)
{
    switch (method)
    {
    case bc_GET:
        return "GET";
        break;
    case bc_HEAD:
        return "HEAD";
        break;
    case bc_POST:
        return "POST";
        break;
    case bc_PUT:
        return "PUT";
        break;
    case bc_DELETE:
        return "DELETE";
        break;
    case bc_CONNECT:
        return "CONNECT";
        break;
    case bc_OPTIONS:
        return "OPTIONS";
        break;
    case bc_TRACE:
        return "TRACE";
        break;
    case bc_PATCH:
        return "PATCH";
        break;
    case bc_NONE:
        return "NONE";
        break;
    default:
        break;
    }
    perror("Unknown method");
    return NULL;
}

struct bc_server_config *bc_server_new(int port)
{
    // Malloc and init the config
    struct bc_server_config *config = malloc(sizeof(struct bc_server_config));
    if (config == NULL)
    {
        perror("bc_server_new: memmory error constructing socket");
        return NULL;
    }
    config->handlers = NULL;
    config->port = port;
    config->host_addrlen = sizeof(struct sockaddr_in);

    // create the host_addr data
    config->host_addr = malloc(sizeof(struct sockaddr_in));
    if (config->host_addr == NULL)
    {
        perror("bc_server_new: memmory error constructing socket_addr struct");
        free(config);
        return NULL;
    }
    config->host_addr->sin_family = AF_INET;
    config->host_addr->sin_port = htons(port);
    config->host_addr->sin_addr.s_addr = htonl(INADDR_ANY);

    // open the socket
    config->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (config->socket_fd < 0)
    {
        perror("bc_server_new: could not create new socket");
        free(config->host_addr);
        free(config);
        return NULL;
    }

    // bind the socket and enable listen for incoming requests
    if (bind(config->socket_fd, (struct sockaddr *)config->host_addr, config->host_addrlen) < 0)
    {
        perror("bc_server_new: could not bind");
        free(config->host_addr);
        free(config);
        return NULL;
    }

    if (listen(config->socket_fd, SOMAXCONN) < 0)
    {
        perror("bc_server_new: could not listen");
        free(config->host_addr);
        free(config);
        return NULL;
    }
    return config;
}

char *bc_request_read_buffer(int accept_fd)
{
    // init the deets 😎
    const int base_size = BC_REQ_BASE_SIZE;
    int cur_size = base_size;
    char *buffer = malloc(sizeof(char) * base_size);
    if (buffer == NULL)
    {
        perror("webserver (malloc)");
        return NULL;
    }

    // read the first chunk
    int valread = read(accept_fd, buffer, base_size);
    if (valread < 0)
    {
        perror("webserver (read)");
        free(buffer);
        return NULL;
    }

    // read the next chunk if needed:
    while (valread == base_size)
    {
        void *result = realloc(buffer, cur_size + base_size);
        if (result == NULL)
        {
            perror("bc_request_read_buffer: Could not realloc");
            return NULL;
        }
        buffer = result;

        valread = read(accept_fd, buffer + cur_size, base_size);
        if (valread < 0)
        {
            perror("bc_request_read_buffer: valread < 0");
            return NULL;
        }
        cur_size = cur_size + base_size;
    }

    int last = cur_size - base_size + valread;
    buffer[last] = '\0';
    return buffer;
}

enum bc_request_method bc_request_parse_method(char *raw)
{
    // the first word of the request should hold the method, so
    // we init our result as bc_NONE and replace when we find a
    // match
    enum bc_request_method result = bc_NONE;

    // get the length of the first word
    int length = 0;
    for (length = 0; length < strlen(raw) && raw[length] != ' '; length++)
    {
    };
    // malloc and copy the word
    char *word = malloc(length + 1);
    strncpy(word, raw, length);
    word[length] = '\0';

    // match the word allowed actions, see MDN docs:
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods
    if (strcmp(word, "GET") == 0)
    {
        result = bc_GET;
    }
    else if (strcmp(word, "HEAD") == 0)
    {
        result = bc_HEAD;
    }
    else if (strcmp(word, "POST") == 0)
    {
        result = bc_POST;
    }
    else if (strcmp(word, "PUT") == 0)
    {
        result = bc_PUT;
    }
    else if (strcmp(word, "DELETE") == 0)
    {
        result = bc_DELETE;
    }
    else if (strcmp(word, "CONNECT") == 0)
    {
        result = bc_CONNECT;
    }
    else if (strcmp(word, "OPTIONS") == 0)
    {
        result = bc_OPTIONS;
    }
    else if (strcmp(word, "TRACE") == 0)
    {
        result = bc_TRACE;
    }
    else if (strcmp(word, "PATCH") == 0)
    {
        result = bc_PATCH;
    }

    // free the word and return our result
    free(word);
    return result;
}

char *bc_request_parse_uri(char *raw)
{
    // the second word of the request should hold uri
    // get the start of the uri
    int start;
    for (start = 0; start < strlen(raw) && raw[start] != ' '; start++)
    {
    };
    // step into the next word
    start++;
    // get the length of the uri
    int length;
    for (length = 0; length < strlen(raw) && raw[start + length] != ' '; length++)
    {
    };
    // malloc and copy the word
    char *word = malloc(length + 1);
    strncpy(word, raw + start, length);
    word[length] = '\0';
    return word;
}

struct bc_request *bc_request_parse(int accept_fd)
{
    struct bc_request *req = malloc(sizeof(struct bc_request));
    req->accept_fd = accept_fd;
    req->raw_buffer = bc_request_read_buffer(req->accept_fd);
    if (req->raw_buffer == NULL)
    {
        perror("bc_request_parse: could not read buffer");
        return NULL;
    }
    req->method = bc_request_parse_method(req->raw_buffer);
    req->uri = bc_request_parse_uri(req->raw_buffer);
    return req;
}

int bc_server_register(struct bc_server_config *config, enum bc_request_method method, char *url, int (*handler)(struct bc_request *req))
{
    char *method_name = bc_method_name(method);
    if (config->handlers == NULL)
    {
        struct dictlist_node *url_rule = dln_new(url, (void *)handler);
        struct dictlist_node *method_rule = dln_new(method_name, (void *)url_rule);
        config->handlers = method_rule;
        return 1;
    }

    struct dictlist_node *method_handler = dln_get(config->handlers, method_name);
    if (method_handler == NULL)
    {
        struct dictlist_node *url_rule = dln_new(url, (void *)handler);
        if (dln_push(config->handlers, method_name, (void *)url_rule) < 0)
        {
            perror("bc_server_register: could not push new method");
            return -1;
        }
        return 1;
    }

    struct dictlist_node *url_rule = dln_get(method_handler, url);
    if (url_rule == NULL)
    {
        dln_push(method_handler, url, (void *)handler);
        return 1;
    }

    perror("bc_server_register: could not push new url");
    return -1;
}

int bc_server_dispatch(struct dispatch_args *args)
{
    struct bc_request *req = bc_request_parse(*args->p_fd);
    if (req == NULL)
    {
        perror("bc_server_dispatch: could not parse req");
        close(*args->p_fd);
        free(args);
        return -1;
    }

    // TODO fix this dispatch
    char *method_name = bc_method_name(req->method);
    struct dictlist_node *url_rules = (struct dictlist_node *)dln_get(args->config->handlers, method_name);
    if (url_rules == NULL)
    {
        perror("bc_server_dispatch: no url_rules for method");
        // clean up request
        close(req->accept_fd);
        free(req->uri);
        free(req->raw_buffer);
        free(req);
        // clean up args
        free(args->p_fd);
        free(args);
        return -1;
    }

    int (*handler)(struct bc_request *req) = dln_get(url_rules, req->uri);
    if (handler == NULL)
    {
        perror("bc_server_dispatch: no handler mapped to url");
        // clean up request
        close(req->accept_fd);
        free(req->uri);
        free(req->raw_buffer);
        free(req);
        // clean up args
        free(args->p_fd);
        free(args);
        return -1;
    }
    handler(req);

    // clean up request
    close(req->accept_fd);
    free(req->uri);
    free(req->raw_buffer);
    free(req);
    // clean up args
    free(args->p_fd);
    free(args);
    return 0;
}

int bc_server_start(struct bc_server_config *config)
{
    // Accept incoming connections
    while (1)
    {
        // malloc each p_fd so that they can pass by refference between threads
        int *p_fd = malloc(sizeof(int));
        *p_fd = accept(config->socket_fd,
                       (struct sockaddr *)config->host_addr,
                       (socklen_t *)&config->host_addrlen);
        if (*p_fd < 0)
        {
            perror("bc_server_start: Could not accept request");
            return -1;
        }

        struct dispatch_args *d_args = malloc(sizeof(struct dispatch_args));
        d_args->config = config;
        d_args->p_fd = p_fd;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, (void *)*bc_server_dispatch, (void *)d_args);
        pthread_detach(thread_id);
    }
}
