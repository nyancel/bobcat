#ifndef BOBCAT_H
#define BOBCAT_H

#include "./util/dictlist.h"

enum bc_request_method
{
    bc_GET = 1,
    bc_HEAD = 2,
    bc_POST = 3,
    bc_PUT = 4,
    bc_DELETE = 5,
    bc_CONNECT = 6,
    bc_OPTIONS = 7,
    bc_TRACE = 8,
    bc_PATCH = 9,
    bc_NONE = 0,
};

struct bc_request
{
    int accept_fd;
    enum bc_request_method method;
    char *uri;
    char *raw_buffer;
};

struct bc_server_config *bc_server_new(int port);
int bc_server_start(struct bc_server_config *config);
int bc_server_register(struct bc_server_config *config, enum bc_request_method method, char *url, int (*handler)(struct bc_request *req));

// utility stuff
char *bc_method_name(enum bc_request_method method);

#endif // BOBCAT_H