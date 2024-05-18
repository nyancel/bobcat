#ifndef SERVER_H
#define SERVER_H

#include "./tcp_socket.h"

struct server_config
{
    struct tcp_socket *tcp_config;
    void (*handler)(int p_fd);
};

int server_start(struct server_config *config);

#endif // SERVER_H