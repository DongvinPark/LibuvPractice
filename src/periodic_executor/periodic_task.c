//
// Created by dongvin on 25. 5. 3.
//
#include "../periodic_executor/periodic_task.h"
#include <stdio.h>
#include <stdlib.h>

struct periodic_task
{
    uv_timer_t timer;
    uv_loop_t *loop;
    uint64_t interval_ms;
    periodic_task_cb callback;
    void* userdata;
    bool running;
    bool is_task_set;
};

// 즉, 재귀적으로 uv_timer_start()를 호출해서 특정 함수의 호출이 주기적으로 반복되게 만든다.
static void on_timer_tick(uv_timer_t* handle)
{
    periodic_task_t* task = (periodic_task_t*)handle->data;

    if (!task->running) return;

    if (task->callback) task->callback(task->userdata);

    if (task->running)
    {
        uv_timer_start( &(task->timer), on_timer_tick, task->interval_ms, 0);
    }
}

periodic_task_t* periodic_task_create(uv_loop_t* loop, uint64_t interval_ms)
{
    periodic_task_t* task = malloc(sizeof(periodic_task_t));
    if (!task) return NULL;

    task->loop = loop;
    task->interval_ms = interval_ms;
    task->callback = NULL;
    task->userdata = NULL;
    task->running = false;
    task->is_task_set = false;

    uv_timer_init(loop, &(task->timer));
    task->timer.data = task;

    return task;
}

void periodic_task_set_callback(periodic_task_t* task, periodic_task_cb cb, void* userdata)
{
    task->callback = cb;
    task->userdata = userdata;
    task->is_task_set = true;
}

void periodic_task_set_interval(periodic_task_t* task, uint64_t interval_ms)
{
    task->interval_ms = interval_ms;
}

void periodic_task_start(periodic_task_t* task)
{
    if (!task->is_task_set)
    {
        fprintf(stderr, "[periodic_task] Error : No task set!\n]");
        return;
    }

    task->running = true;
    uv_timer_start( &(task->timer), on_timer_tick, task->interval_ms, 0);
}

void periodic_task_stop(periodic_task_t* task)
{
    if (!task->is_task_set) return;

    fprintf(stderr, "[periodic_task] Error : Task stopped!\n");
    task->running = false;
    uv_timer_stop( &(task->timer));
}

void periodic_task_destroy(periodic_task_t* task)
{
    if (task->running)
    {
        periodic_task_stop(task);
    }
    free(task);
}
