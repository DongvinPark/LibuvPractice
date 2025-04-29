//
// Created by 박동빈 on 2025. 4. 29..
//

#ifndef DELAYED_EXECUTOR_H
#define DELAYED_EXECUTOR_H

#include <uv.h>

// 딜레이 후 실행할 태스크 함수를 전달하기 위한, '함수 포인터'
typedef void (*delayed_task_cb)(void* arg);

// public API : 호출자는 이 함수롤 이용해서 특정 함수를 특정 밀리초만큼 시간이 지난 후에 실행시킨다.
void delayed_executor_async(uv_loop_t* loop, int delay_in_ms, delayed_task_cb cb, void* arg);

#endif //DELAYED_EXECUTOR_H
