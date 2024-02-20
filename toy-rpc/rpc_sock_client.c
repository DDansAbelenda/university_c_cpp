#include <stdio.h>
#include <string.h>

#include "sock_helper.h"
#include "librio/rio.h"

int main(int argc, char *argv[])
{
    int clientfd;
    rio_t rio;
    char buf[1024];

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    clientfd = open_clientfd(argv[1], argv[2]);

    if (clientfd < 0)
    {
        fprintf(stderr, "Failed to connect to the requested server.\n");
        exit(1);
    }

    fprintf(stdout, "Connected to server %s:%s\n", argv[1], argv[2]);
    rio_readinitb(&rio, clientfd);

    fprintf(stdout, "> ");

    while (fgets(buf, 1024, stdin) != NULL)
    {
        int written = rio_writen(clientfd, buf, strlen(buf));
        int read = rio_readlineb(&rio, buf, 1024);

        if (read)
        {
            fputs(buf, stdout);

            if (!strncmp(buf, "Goodbye", 7))
                break;
        }
        else
        {
            break;
        }

        fprintf(stdout, "> ");
    }

    close(clientfd);
    exit(0);
}