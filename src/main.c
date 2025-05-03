//
// Created by dongvin on 25. 4. 27.
//
#include <stdio.h>
#include <uv.h>

#include "../src/util/util.h"
#include "periodic_executor/periodic_task.h"
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

/*
Opaque Struct 라는 패턴을 이용해서 C 언어에서 OOP(Object Oriented Programming)를 따라해보았다.

데이터 멤버를 담을 struct와 해당 객체에서 수행 가능한 연산들인 함수들을 periodic_task.h에 선언하고,
구체적인 정의는 periodic_task.c에서 구현하였다.

1초 주기로 수행하는 일을 10초 동안 수행하다가 정지 된다.
10초가 됐을 때 바로 정지 시키기 때문에, periodic_task는 1 번째 ~ 9 번째 까지만 수행된다.

async nonblocking delayed task를 굳이 여기에서 코드를 복붙해서 활용한 이유는,
periodic task를 중간에 멈출 수단이 필요했기 때문이다.
중간에 멈추지 않는다면, periodic task를 프로그램을 강제로 종료하지 않는 이상 영원히 수행된다.
 */
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