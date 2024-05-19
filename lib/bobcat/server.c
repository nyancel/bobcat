#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "./tcp_socket.h"
#include "./server.h"

struct dispatch_args
{
    struct server_config *config;
    int p_fd;
};

int server_dispatch(struct dispatch_args *args)
{
    args->config->handler(args->p_fd);
    free(args);
    return 0;
}

int server_start(struct server_config *config)
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
        pthread_create(&thread_id, NULL, (void *)*server_dispatch, (void *)d_args);
        pthread_detach(thread_id);
    }
}
