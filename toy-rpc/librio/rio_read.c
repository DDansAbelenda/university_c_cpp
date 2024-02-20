#include "rio_priv.h"

ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;
    char c;

    while (rp->rio_cnt <= 0)
    {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));

        if (rp->rio_cnt < 0)
        {
            if (errno != EINTR)
            {
                return -1;
            }
        }
        else if (rp->rio_cnt == 0)
        {
            return 0;
        }
        else
        {
            rp->rio_bufptr = rp->rio_buf;
        }
    }

    cnt = n;

    if (rp->rio_cnt < n)
    {
        cnt = rp->rio_cnt;
    }

    memcpy(usrbuf, rp->rio_bufptr, cnt);

    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;

    return cnt;
}