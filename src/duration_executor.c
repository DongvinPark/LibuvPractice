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

// 이러한 종료 핸들러를 이용해서 자원을 할당 해제하는 것이 안전하다.
// 종료 핸들러 없이 on_timer() 내부의 마지막 부분에서 그냥 바로 free(context);
// 해버리면 윈도우 OS의 libuv에서는 context를 아직 사용 중인데 갑자기 할당 해제를 해버리면서
// Assertion failed: 0, file C:\vcpkg\buildtrees\libuv\src\v1.49.2-5cffa54d5b.clean\src\win\handle-inl.h, line 160
// 라는 에러 메시지와 함께 프로그램이 종료돼 버린다.
static void on_close(uv_handle_t* handler) {
    duration_executor_context_t* context = (duration_executor_context_t*)handler->data;
    free(context);
}

static void on_timer(uv_timer_t* handle)
{
    duration_executor_context_t* context = (duration_executor_context_t*)handle->data;
    printf("Duration expired - stopping task...\n");

    uv_idle_stop( &(context->idle) );

    if (!uv_is_closing( (uv_handle_t*)&(context->idle) ))
    {
        uv_close( (uv_handle_t*)&(context->idle), NULL);
    }

    if (!uv_is_closing( (uv_handle_t*)&(context->timer) ))
    {
        uv_close( (uv_handle_t*)&(context->timer), on_close);
    }
}

void async_nonblocking_duration_executor(
    uv_loop_t* loop, int duration_ms, duration_executor_cb cb, void* arg)
{
    duration_executor_context_t* ctx_ptr = malloc(sizeof(duration_executor_context_t));
    ctx_ptr->cb = cb;
    ctx_ptr->arg = arg;

    uv_idle_init(loop, &(ctx_ptr->idle) );
    ctx_ptr->idle.data = ctx_ptr; // libuv 이벤트 루프 내부에서 duration_executor_context_t 객체에 접근할 수 있는 수단을 제공해야 한다.
    uv_idle_start( &(ctx_ptr->idle), on_idle);

    uv_timer_init(loop, &(ctx_ptr->timer) );
    ctx_ptr->timer.data = ctx_ptr; // libuv 이벤트 루프 내부에서 duration_executor_context_t 객체에 접근할 수 있는 수단을 제공해야 한다.
    uv_timer_start( &(ctx_ptr->timer), on_timer, duration_ms, 0);
}
