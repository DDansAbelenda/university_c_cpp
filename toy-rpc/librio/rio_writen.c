#include "rio_priv.h"

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    char *bufp = usrbuf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) <= 0)
        {
            if (errno == EINTR)
            {
                nwritten = 0;
            }
            else
            {
                return -1;
            }
        }

        nleft -= nwritten;
        bufp += nwritten;
    }

    return n;
}
