#include "../rpc.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    reportable_t parent;

    long unsigned int op1;
    long unsigned int op2;
    long unsigned int res;
} my_struct_t;

void *parse_parameters(void *data)
{
    const char *buf = (const char *)(data);

    my_struct_t *d = (my_struct_t *)(malloc(sizeof(my_struct_t)));

    if (d)
    {
        sscanf(buf, "%ld %ld", &d->op1, &d->op2);
    }

    return (void *)d;
}

void *do_work(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    long unsigned int _a = d->op1;
    long unsigned int _b = d->op2;
    long unsigned int _t = _b;

    if (_a)
    {
        while (_b)
        {
            _b = _a % _t;
            _a = _t;
            _t = _b;
        }

        if (d->op1 > d->op2)
        {
            d->res = d->op1 / _a * d->op2;
        }
        else
        {
            d->res = d->op2 / _a * d->op1;
        }
    }

    return data;
}

reportable_t *report(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    d->parent.data = (char *)(malloc(255 * sizeof(char)));

    snprintf(d->parent.data, 255, "LCM(%ld,%ld) = %ld\n", d->op1, d->op2, d->res);
    d->parent.len = strlen(d->parent.data);

    return (reportable_t *)(data);
}

void clean_up(void *params, void *result, reportable_t *report)
{
    if (report && report->data)
    {
        free(report->data);
    }

    if (params)
    {
        free(params);
    }
}
