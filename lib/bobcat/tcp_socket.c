#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "./tcp_socket.h"

struct tcp_socket *tcp_socket_new(int port)
{
    // Malloc everything
    struct tcp_socket *config = malloc(sizeof(struct tcp_socket));
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

int tcp_socket_bind(struct tcp_socket *config)
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

int tcp_socket_listen(struct tcp_socket *config)
{
    if (listen(config->socket_fd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        return -1;
    }
    printf("server listening for connections\n");
    return 0;
}

int tcp_socket_free(struct tcp_socket *config)
{
    close(config->socket_fd);
    free(config->host_addr);
    free(config);
}