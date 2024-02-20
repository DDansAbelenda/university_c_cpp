#include "threadpool.h"
#include "librio/rio.h"

struct worker_context
{
    callback_t action;
    void *data;
    worker_context_t *next;
};

struct threadpool
{
    pthread_t *threads;
    worker_context_t *jobs, *head;

    unsigned int count;
    unsigned char running;

    pthread_cond_t jobs_cond;
    pthread_mutex_t mutex;
};

static void *__thread_fn(void *);

threadpool_t *pool_create(unsigned int worker_count)
{
    if (worker_count > MAX_THREADS)
        worker_count = MAX_THREADS;

    threadpool_t *pool = NULL;

    if (!(pool = (threadpool_t *)(malloc(sizeof(threadpool_t)))))
        goto failed;

    pool->count = worker_count;

    if (!(pool->threads = (pthread_t *)(malloc(worker_count * sizeof(pthread_t)))))
        goto free_pool;

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->jobs_cond, NULL);

    pool->jobs = NULL;

    for (int i = 0; i < pool->count; i++)
    {
        pthread_create(&pool->threads[i], NULL, __thread_fn, (void *)pool);
    }

    pool->running = 1;

    return pool;

free_threads:
    free(pool->threads);
free_pool:
    free(pool);
failed:
    return NULL;
}

void pool_stop(threadpool_t *pool)
{
    pthread_mutex_lock(&pool->mutex);

    pool->running = 0;
    pthread_cond_broadcast(&pool->jobs_cond);

    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < pool->count; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }
}

void pool_destroy(threadpool_t *pool)
{
    if (pool == NULL)
    {
        return;
    }

    worker_context_t *i = pool->jobs;
    worker_context_t *j = pool->head;
    worker_context_t *k;

    while (i && i <= j)
    {
        k = i;
        i = i->next;

        free(k);
    }

    free(pool->threads);
}

int pool_submit_work(
    threadpool_t *pool,
    callback_t action,
    void *data)
{
    int res = -1;
    worker_context_t *w;

    pthread_mutex_lock(&pool->mutex);

    if (pool->running && (w = (worker_context_t *)(malloc(sizeof(worker_context_t)))))
    {
        w->action = action;
        w->data = data;
        w->next = NULL;

        if (pool->jobs == NULL)
        {
            pool->jobs = pool->head = w;
        }
        else
        {
            pool->head->next = w;
            pool->head = w;
        }

        pthread_cond_signal(&pool->jobs_cond);
    }

    pthread_mutex_unlock(&pool->mutex);

    return res;
}

static worker_context_t *pool_get_job(threadpool_t *pool)
{
    worker_context_t *p = NULL;

    pthread_mutex_lock(&pool->mutex);

    while (pool->jobs == NULL && pool->running)
    {
        pthread_cond_wait(&pool->jobs_cond, &pool->mutex);
    }

    if (pool->running)
    {
        p = pool->jobs;
        pool->jobs = p->next;
    }

    pthread_mutex_unlock(&pool->mutex);

    return p;
}

static void *__thread_fn(void *args)
{
    threadpool_t *pool = (threadpool_t *)(args);
    worker_context_t *job;

    while (job = pool_get_job(pool))
    {
        job->action(job->data);
        free(job);
    }
}
