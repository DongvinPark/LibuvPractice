//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <uv.h>

#include "../src/util/util.h"
#include "../src/periodic_task.h"
#include "../src/delayed_executor/delayed_executor.h"

void my_task_callback(void* userdata)
{
    int* counter = (int*)userdata;
    (*counter)++;
    char time_str[32];
    get_current_utc_time_string(time_str, sizeof(time_str));
    printf("Task called! count : %d, time : %s\n", *counter, time_str);
}

void quit_my_task(void* arg)
{
    periodic_task_t* task = (periodic_task_t*)arg;
    periodic_task_stop(task);
}

int main() {
    uv_loop_t* loop = uv_default_loop();
    int counter = 0;

    periodic_task_t* task = periodic_task_create(loop, 1000);
    periodic_task_set_callback(task, my_task_callback, &counter);
    periodic_task_start(task);

    async_nonblocking_delayed_executor(loop, 10000, quit_my_task, task);

    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}