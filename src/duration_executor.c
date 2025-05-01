//
// Created by dongvin on 25. 5. 1.
//

#include <stdlib.h>
#include <stdio.h>

#include "../src/duration_executor.h"

typedef struct
{
    uv_idle_t idle;
    uv_timer_t timer;
    duration_executor_cb cb;
    void* arg;
} duration_executor_context_t;

static void on_idle(uv_idle_t* handle)
{
    duration_executor_context_t* context = (duration_executor_context_t*)handle->data;
    if (context->cb)
    {
        context->cb(context->arg);
    }
}

static void on_timer(uv_timer_t* handle)
{
    duration_executor_context_t* context = (duration_executor_context_t*)handle->data;
    printf("Duration expired - stopping task...\n");

    uv_idle_stop( &(context->idle) );
    uv_close( (uv_handle_t*)&(context->idle), NULL);
    uv_close( (uv_handle_t*)&(context->timer), NULL);

    free(context);
}

void async_nonblocking_duration_executor(
    uv_loop_t* loop, int duration_ms, duration_executor_cb cb, void* arg)
{
    duration_executor_context_t* ctx_ptr = malloc(sizeof(duration_executor_context_t));
    ctx_ptr->cb = cb;
    ctx_ptr->arg = arg;

    uv_idle_init(loop, &(ctx_ptr->idle) );
    ctx_ptr->idle.data = ctx_ptr;
    uv_idle_start( &(ctx_ptr->idle), on_idle);

    uv_timer_init(loop, &(ctx_ptr->timer) );
    ctx_ptr->timer.data = ctx_ptr;
    uv_timer_start( &(ctx_ptr->timer), on_timer, duration_ms, 0);
}
