#ifndef BOBCAT_H
#define BOBCAT_H

enum bc_request_method
{
    GET = 1,
    HEAD = 2,
    POST = 3,
    PUT = 4,
    DELETE = 5,
    CONNECT = 6,
    OPTIONS = 7,
    TRAFCE = 8,
    PATCH = 9,
    NONE = 0,
};

struct bc_request
{
    int accept_fd;
    enum bc_request_method method;
    char *uri;
    char *raw_buffer;
};

struct bc_tcp_socket
{
    int port;
    int socket_fd;
    int host_addrlen;
    struct sockaddr_in *host_addr;
};

struct bc_server_config
{
    struct bc_tcp_socket *tcp_config;
    int (*handler)(struct bc_request *req);
};

struct bc_server_config *bc_server_new(int port);
int bc_server_start(struct bc_server_config *config);

#endif // BOBCAT_H