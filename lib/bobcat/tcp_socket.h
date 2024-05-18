#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

struct tcp_socket
{
    int port;
    int socket_fd;
    int host_addrlen;
    struct sockaddr_in *host_addr;
};

struct tcp_socket *tcp_socket_new(int port);
int tcp_socket_bind(struct tcp_socket *config);
int tcp_socket_listen(struct tcp_socket *config);
int tcp_socket_free(struct tcp_socket *config);

#endif // TCP_SOCKET_H