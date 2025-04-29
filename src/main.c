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

/*
1, 2, 4초 딜레이 후 실행되는 async non-blocking 태스크 3 개를 libuv를 이용해서 실행시킨다.
실행 결과는 아래와 같다. 로그를 보면, 태스크이 실행 완료보다,
태스크 3 개를 libuv에 던져 놓는(즉, dispatching) 것을 완료했다는 로그가 더 빨리 뜨는 것을 알 수 있다.

/Users/dongvin99/Documents/LibuvPractice/cmake-build-debug/LibuvPractice
Starting loop...
Setting tasks is done!

Delayed task executed! Time = I'm the first!
time to exit timer! exit time : UTC2025_04_29T04_13_53.

Delayed task executed! Time = I'm the second!
time to exit timer! exit time : UTC2025_04_29T04_13_54.

Delayed task executed! Time = I'm the third!
time to exit timer! exit time : UTC2025_04_29T04_13_56.

Event loop exited.

Process finished with exit code 0

 */
int main() {
    uv_loop_t* loop = uv_default_loop();
    printf("Starting loop...\n");

    async_nonblocking_delayed_executor(loop, 1000, my_task_1, "I'm the first!");
    async_nonblocking_delayed_executor(loop, 2000, my_task_2, "I'm the second!");
    async_nonblocking_delayed_executor(loop, 4000, my_task_3, "I'm the third!");
    printf("Setting tasks is done!\n\n");

    uv_run(loop, UV_RUN_DEFAULT);

    printf("Event loop exited.\n");

    return 0;
}