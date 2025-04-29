//
// Created by dongvin on 25. 4. 27.
//
#include <uv.h>

#include "delayed_executor.h"

void my_task_1(void* arg)
{
    printf("Delayed task executed! Time = %s\n", (char*)arg);
}

void my_task_2(void* arg)
{
    printf("Delayed task executed! Time = %s\n", (char*)arg);
}

void my_task_3(void* arg)
{
    printf("Delayed task executed! Time = %s\n", (char*)arg);
}

int main() {
    uv_loop_t* loop = uv_default_loop();
    printf("Starting loop...\n");

    delayed_executor_async(loop, 1000, my_task_1, "I'm the first!");
    delayed_executor_async(loop, 2000, my_task_2, "I'm the second!");
    delayed_executor_async(loop, 4000, my_task_3, "I'm the third!");
    uv_run(loop, UV_RUN_DEFAULT);

    printf("Event loop exited.\n");

    return 0;
}