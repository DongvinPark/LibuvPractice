//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for int64_t
#include <time.h>
#include <inttypes.h> // to print int64_t value correctly in printf() function.
#include <uv.h>

typedef struct
{
    uv_idle_t idle;
    uv_timer_t timer;
    int work_counter;

} task_context_t;

int64_t get_current_time_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)(ts.tv_sec * 1000LL + ts.tv_nsec / 1000000);
}

void stop_task(uv_timer_t* handle)
{
    task_context_t* ctx = (task_context_t*)handle->data;
    printf("Stopping task after timer! current time millis : %" PRId64 "\n", get_current_time_millis());

    uv_idle_stop(&ctx->idle);
    uv_close((uv_handle_t*)&ctx->idle, NULL);
    uv_close((uv_handle_t*)&ctx->timer, NULL);
}

void run_task(uv_idle_t* handle)
{
    task_context_t* ctx = (task_context_t*)handle->data;

    for (int i = 0; i < 1000; i++)
    {
        ctx->work_counter++;
    }
}

int main()
{
    uv_loop_t* loop = uv_default_loop();

    task_context_t* ctx = malloc(sizeof(task_context_t));

    uv_idle_init(loop, &ctx->idle);
    ctx->idle.data = ctx;
    uv_idle_start(&ctx->idle, run_task);

    uv_timer_init(loop, &ctx->timer);
    ctx->timer.data = ctx;
    uv_timer_start(&ctx->timer, stop_task, 3000, 0);

    // int64_t 타입 값을 정확하게 출력하려면, %d 나 %ld 같은 거 쓰면 안 된다.
    printf("Starting task for 3000ms ... current time millis : %" PRId64 "\n", get_current_time_millis());
    uv_run(loop, UV_RUN_DEFAULT);

    free(ctx);
    return 0;
}




















