#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../lib/bobcat.h"
#include "../lib/util/dictlist.h"

int server_request_handler(struct bc_request *req)
{
    // plain example for now, should improve later
    int request_fd = req->accept_fd;

    printf("connection accepted\n");
    char buffer[1024];
    char *resp = "HTTP/1.0 200 OK\r\n"
                 "Server: webserver-c\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<html>hello, world</html>\r\n";

    int valread = read(request_fd, buffer, 1024);
    if (valread < 0)
    {
        perror("webserver (read)");
        return -1;
    }

    // Write to the socket
    int valwrite = write(request_fd, resp, strlen(resp));
    if (valwrite < 0)
    {
        perror("webserver (write)");
        return -1;
    }

    close(request_fd);
    return 0;
};

int main(int argc, char *argv)
{
    // clean the terminal when we start the server
    system("clear");
    struct bc_server_config *serv_con = bc_server_new(3000);
    serv_con->handler = (int *)&server_request_handler;
    // start the server
    bc_server_start(serv_con);
    return 0; // <- should never actually return since server_start is blocking
}
