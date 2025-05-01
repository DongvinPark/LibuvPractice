//
// Created by dongvin on 25. 5. 1.
//

#ifndef DURATION_EXECUTOR_H
#define DURATION_EXECUTOR_H

#include <uv.h>

// 특정 기간동한 실행할 함수를 전달하기 위한 '함수 포인터'
typedef void (*duration_executor_cb)(void* arg);

// public API : 호출자는 이 함수를 이용해서 특정 함수를 특정 기간 동안만 실행시킬 수 있다.
void async_nonblocking_duration_executor(uv_loop_t* loop, int duration_ms, duration_executor_cb cb, void* arg);

#endif //DURATION_EXECUTOR_H
