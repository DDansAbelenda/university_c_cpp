#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 8

typedef struct worker_context worker_context_t;
typedef struct threadpool threadpool_t;
typedef void *(*callback_t)(void *data);

threadpool_t *pool_create(unsigned int worker_count);
void pool_destroy(threadpool_t *pool);

int pool_submit_work(threadpool_t *pool, callback_t action, void *data);
void pool_stop(threadpool_t *pool);

#endif