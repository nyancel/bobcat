#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "./bobcat.h"

struct dispatch_args
{
    struct bc_server_config *config;
    int p_fd;
};

struct bc_tcp_socket *tcp_socket_new(int port)
{
    // Malloc everything
    struct bc_tcp_socket *config = malloc(sizeof(struct bc_tcp_socket));
    if (config == NULL)
    {
        perror("memmory error constructing socket");
        return NULL;
    }

    config->host_addr = malloc(sizeof(struct sockaddr_in));
    if (config->host_addr == NULL)
    {
        perror("memmory error constructing socket_addr");
        free(config);
        return NULL;
    }

    // open the socket
    config->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (config->socket_fd == -1)
    {
        perror("could not create new socket");
        free(config->host_addr);
        free(config);
        return NULL;
    }

    printf("TCP socket created\n");
    // assign remaining stuff;
    config->host_addrlen = sizeof(struct sockaddr_in);
    config->port = port;
    config->host_addr->sin_family = AF_INET;
    config->host_addr->sin_port = htons(port);
    config->host_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    printf("TCP socket configured\n");
    return config;
}

int tcp_socket_bind(struct bc_tcp_socket *config)
{
    // Bind the socket to the address
    if (bind(config->socket_fd, (struct sockaddr *)config->host_addr, config->host_addrlen) != 0)
    {
        perror("webserver (bind)");
        return -1;
    }
    printf("socket successfully bound to address\n");
    return 0;
}

int tcp_socket_listen(struct bc_tcp_socket *config)
{
    if (listen(config->socket_fd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        return -1;
    }
    printf("server listening for connections\n");
    return 0;
}

int tcp_socket_free(struct bc_tcp_socket *config)
{
    close(config->socket_fd);
    free(config->host_addr);
    free(config);
}

struct bc_server_config *bc_server_new(int port)
{
    struct bc_tcp_socket *s = tcp_socket_new(port);
    tcp_socket_bind(s);
    tcp_socket_listen(s);
    struct bc_server_config *server_config = malloc(sizeof(struct bc_server_config));
    server_config->tcp_config = s;
    return server_config;
}

int bc_server_dispatch(struct dispatch_args *args)
{
    args->config->handler(args->p_fd);
    free(args);
    return 0;
}

int bc_server_start(struct bc_server_config *config)
{
    // Accept incoming connections
    while (1)
    {
        int p_fd = accept(config->tcp_config->socket_fd,
                          (struct sockaddr *)config->tcp_config->host_addr,
                          (socklen_t *)&config->tcp_config->host_addrlen);
        if (p_fd < 0)
        {
            perror("webserver (accept)");
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
