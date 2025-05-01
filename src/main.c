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
    // 여기에는 while(true){...} 같은 무한 루프 넣지 않는다.
    // uv_idle_t를 이용하면 while(true){...} 같은 무한 루프 없이도 일정시간 동안
    // 이 함수를 반복 실행할 수 있다.
    int64_t* counter_ptr = (int64_t*)arg;
    (*counter_ptr)++;
}

/*
특정 함수를 특정한 지속시간 동안만 실행시키는 예제다.
delayed executor는 특정 작업을 특정한 시점에 단 1 번만 호출하면 되지만,
일정 시간동안 태스크를 지속적으로 실행해야 하는 상황에서는
libuv의 'timer' 뿐만 아니라 'uv_idle_t'이라는 것도 추가로 활용해야 한다.

uv_idle_t를 활용하지 않고, 단순히 while(true){...} 같은걸로 구현하면
libuv의 이벤트 루프가 해당 태스크 때문에 blocking 되고 만다.

uv_idle_t를 사용하면, event loop 마다 타깃 함수를 특별히 급한 일이 있지 않은 이상
최우선적으로 실행시킨다.
libuv에게 이렇게 말하는 것과 같다.
"Run this callback as fast as possible while the loop is idle (i.e., has no other higher-priority I/O to do)."
 */
int main()
{
    uv_loop_t* loop = uv_default_loop();

    int64_t counter = 0L;
    int64_t* counter_ptr = &counter;
    async_nonblocking_duration_executor(loop, 3000, run_add_task, counter_ptr);
    // int64_t 타입 값을 정확하게 출력하려면, %d 나 %ld 같은 거 쓰면 안 된다.
    printf("Starting task for 3000ms ... current time millis : %" PRId64 "\n", get_current_time_millis());
    uv_run(loop, UV_RUN_DEFAULT);

    printf("Add task result : %" PRId64 "\n", counter);
    printf("Exit time : %" PRId64 "\n", get_current_time_millis());
    return 0;
}




















