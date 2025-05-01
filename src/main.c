//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // for int64_t
#include <time.h>
#include <inttypes.h> // to print int64_t value correctly in printf() function.
#include <uv.h>

#include "duration_executor.h"


int64_t get_current_time_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)(ts.tv_sec * 1000LL + ts.tv_nsec / 1000000);
}

void run_add_task(void* arg)
{
    int64_t* counter_ptr = (int64_t*)arg;
    (*counter_ptr)++;
}

int main()
{
    uv_loop_t* loop = uv_default_loop();

    int64_t counter = 0L;
    int64_t* counter_ptr = &counter;

    // int64_t 타입 값을 정확하게 출력하려면, %d 나 %ld 같은 거 쓰면 안 된다.
    printf("Starting task for 3000ms ... current time millis : %" PRId64 "\n", get_current_time_millis());
    async_nonblocking_duration_executor(loop, 3000, run_add_task, counter_ptr);
    uv_run(loop, UV_RUN_DEFAULT);

    printf("Add task result : %" PRId64 "\n", counter);
    printf("Exit time : %" PRId64 "\n", get_current_time_millis());
    return 0;
}




















