#ifndef REQUEST_H
#define REQUEST_H

enum request_method
{
    GET = 1,
    POST = 0,
    UNKNOWN = -1,
};

struct request
{
    enum request_method method;
    char *uri;
    char *raw_buffer;
};

#endif // REQUEST_H