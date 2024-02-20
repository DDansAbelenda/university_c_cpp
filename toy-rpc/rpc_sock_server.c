#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "sock_helper.h"
#include "librio/rio.h"
#include "threadpool.h"
#include "rpc.h"

#define MAXPROC 64

static int loadedprocs = 0;
static int working = 0;
static pthread_mutex_t proc_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;
static context_t procedures[MAXPROC];

static context_t *get_context_by_name(const char *name, size_t len)
{
    for (int i = 0; i < loadedprocs; i++)
    {
        fprintf(stdout, "[%s] vs [%s]\n", procedures[i].name, name);

        if (!strncasecmp(procedures[i].name, name, len))
        {
            return &procedures[i];
        }
    }

    return NULL;
}

static context_t *load_procedure(const char *name, size_t len)
{
    char filename[1024];

    // name = "jsmith"
    // filename = "./dynamic/jsmith.so"

    snprintf(filename, 14 + len, "./dynamic/%s.so", name);

    fprintf(stdout, "Looking for [%s] (%s)\n", filename, name);

    context_t *c = get_context_by_name(name, len);

    fprintf(stdout, "Found it? = %s\n", !c ? "no" : "yes");

    if (c != NULL)
    {
        return c;
    }

    if (loadedprocs >= MAXPROC)
    {
        return NULL;
    }

    c = &procedures[loadedprocs];

    if (!(c->lib_handle = dlopen(filename, RTLD_LAZY)))
    {
        fprintf(stderr, "Failed to open dynamic library: %s\n", filename);

        return NULL;
    }
    else if (
        !(c->do_work = dlsym(c->lib_handle, "do_work")) ||
        !(c->parse_parameters = dlsym(c->lib_handle, "parse_parameters")) ||
        !(c->clean_up = dlsym(c->lib_handle, "clean_up")) ||
        !(c->report = dlsym(c->lib_handle, "report")))
    {
        fprintf(stderr, "Missing required implementation\n");
        dlclose(c->lib_handle);

        return NULL;
    }
    else
    {
        loadedprocs++;
        memcpy(c->name, name, len);
    }

    fprintf(stdout, "New procedure loaded: %s\n", name);

    return c;
}

void *run_connection(void *data)
{
    int fd = *((int *)data);
    int read = 0;
    char buf[1024];

    void *params;

    rio_t rio;
    rio_readinitb(&rio, fd);

    while (1)
    {
        read = rio_readlineb(&rio, buf, 1024);

        if (read <= 0)
        {
            close(fd);

            break;
        }
        else if (!strncmp(buf, "exit", 4))
        {
            rio_writen(fd, "Goodbye\n", 9);
            close(fd);

            break;
        }

        int i;
        for (i = 0; i < read; i++)
        {
            char c = buf[i];

            if (c == 0 || c == '\n' || c == ' ')
                break;
        }

        buf[i] = buf[read - 1] = 0;
        params = (void *)(buf + i + 1);

        pthread_mutex_lock(&proc_mutex);

        context_t *ctx = load_procedure(buf, i);

        pthread_mutex_unlock(&proc_mutex);

        if (!ctx)
        {
            fprintf(stdout, "Client request could not be processed\n");
            rio_writen(fd, "Failed to find procedure to call.\n", 35);

            continue;
        }

        void *parsed = ctx->parse_parameters(params);
        void *result = ctx->do_work(parsed);
        reportable_t *to_report = ctx->report(result);

        rio_writen(fd, to_report->data, to_report->len);

        ctx->clean_up(parsed, result, to_report);
        parsed = result = to_report = NULL;
    }

    pthread_mutex_lock(&accept_mutex);

    working--;

    pthread_mutex_unlock(&accept_mutex);
}

int main(int argc, char *argv[])
{
    char buf[1024];
    char filename[1024];

    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = open_listenfd(argv[1]);

    if (listenfd < 0)
    {
        fprintf(stderr, "Failed to start listening.\n");
        exit(1);
    }
    else
    {
        fprintf(stdout, "Server listening on port %s.\nWaiting for clients...\n", argv[1]);
    }

    threadpool_t *pool = pool_create(8);

    if (!pool)
    {
        fprintf(stderr, "Failed to initialize server thread pool.\n");
        exit(1);
    }

    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        char accepted = 1;

        pthread_mutex_lock(&accept_mutex);

        if ((accepted = (working < 8)))
        {
            int *o = malloc(sizeof(int));
            *o = connfd;

            if (pool_submit_work(pool, run_connection, (void *)o) != -1)
            {
                working++;
            }
        }

        pthread_mutex_unlock(&accept_mutex);

        if (!accepted)
        {
            rio_writen(connfd, "Server is unavailable\n", 23);
            close(connfd);
        }
    }

    fprintf(stdout, "Waiting for thread pool deinit...\n");

    pool_stop(pool);
    pool_destroy(pool);

    for (int i = 0; i < loadedprocs; i++)
    {
        dlclose(procedures[i].lib_handle);
    }

    close(listenfd);

    exit(0);
}