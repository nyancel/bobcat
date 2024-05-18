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

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, (void *)config->handler, (void *)&p_fd);
        pthread_detach(thread_id);
    }
}
