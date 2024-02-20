#include "rio_priv.h"

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread = 0;
    char *bufp = usrbuf;

    while (nleft > 0)
    {
        if ((nread = read(fd, bufp, nleft) < 0))
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
        {
            break;
        }

        nleft -= nread;
        bufp += nread;
    }

    return (n - nleft);
}