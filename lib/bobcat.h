#ifndef BOBCAT_H
#define BOBCAT_H

struct bc_server_config
{
    struct bc_tcp_socket *tcp_config;
    void (*handler)(int p_fd);
};

struct bc_tcp_socket
{
    int port;
    int socket_fd;
    int host_addrlen;
    struct sockaddr_in *host_addr;
};

struct bc_server_config *bc_server_new(int port);
int bc_server_start(struct bc_server_config *config);

#endif // BOBCAT_H