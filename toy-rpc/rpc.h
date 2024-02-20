#ifndef __SIMPLE_RPC_H__
#define __SIMPLE_RPC_H__

#include "threadpool.h"

typedef struct
{
    void *data;
    int len;
} reportable_t;

typedef reportable_t *(*reporter_t)(void *);
typedef void (*clean_up_t)(void *params, void *result, reportable_t *report);

typedef struct
{
    callback_t parse_parameters;
    callback_t do_work;
    clean_up_t clean_up;
    reporter_t report;

    char name[128];

    void *lib_handle;
} context_t;

#endif