#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../lib/bobcat.h"
#include "../lib/util/dictlist.h"

int server_request_handler(struct bc_request *req)
{
    printf("URI was: %s\n", req->uri);
    printf("METHOD was: %s\n", bc_method_name(req->method));
    printf("RAW REQUEST\n__________\n%s\n", req->raw_buffer);

    // plain example for now, should improve later
    char *resp = "HTTP/1.0 200 OK\r\n"
                 "Server: webserver-c\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<html>hello, world</html>\r\n";

    // Write to the socket
    int valwrite = write(req->accept_fd, resp, strlen(resp));
    if (valwrite < 0)
    {
        perror("webserver (write)");
        return -1;
    }
    return 0;
};

int main(int argc, char *argv)
{
    // clean the terminal when we start the server
    system("clear");

    // create the server config and TCP allocation stuff ++
    struct bc_server_config *serv_con = bc_server_new(3000);
    if (serv_con == NULL)
    {
        perror("main: Could not init the server");
        return -1;
    }

    // register the handler
    // serv_con->handler = (void *)&server_request_handler;
    if (bc_server_register(serv_con, bc_GET, "/", (void *)&server_request_handler) < 0)
    {
        perror("main: Could not register route");
        return -1;
    }

    // start the server
    bc_server_start(serv_con);
    return 0; // <- should never actually return since server_start is blocking
}
