//
// Created by 박동빈 on 2025. 4. 29..
//

#include "../src/delayed_executor.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    uv_timer_t timer_handle;
    delayed_task_cb cb;
    void* arg;
} delayed_task_t;

void get_current_utc_time_string(char* buffer, size_t buffer_size)
{
    time_t now = time(NULL);
    struct tm utc_time;

#ifdef _WIN32
    gmtime_s(&utc_time, &now);
#else
    struct tm* tmp = gmtime(&now);
    if (tmp)
    {
        utc_time = *tmp;
    } else
    {
        memset(&utc_time, 0, sizeof(struct tm));
    }
#endif

    strftime(buffer, buffer_size, "UTC%Y_%m_%dT%H_%M_%S", &utc_time);
}

static void on_timer_close(uv_handle_t* handle_ptr)
{
    delayed_task_t* task = (delayed_task_t*)handle_ptr->data;
    // task 객체를 해제한다.
    free(task);
}

// 이 함수는 미리 설정된 '지연 시간'이 전부 지나간 후 libuv에 의해서 호출되는 함수다.
static void on_timer_fire(uv_timer_t* handle_ptr)
{
    // delayed_executor_async 함수 내부에서
    // task->timer_handle.data = task;
    // 위의 주석 친 코드에서 초기화한 data 필드를 아래의 코드에서 delayed_task_t* 포인터로 형변환 해서
    // 비로서 libuv 에서 task에 접근할 수 있게 된다.
    delayed_task_t* task = (delayed_task_t*)handle_ptr->data;
    if (task && task->cb)
    {
        // 지연시간이 다 지났으므로, 넘겨 받는 인자를 전달하면서 실제로 함수를 호출한다.
        task->cb(task->arg);
    }

    char time_str[32];
    get_current_utc_time_string(time_str, sizeof(time_str));
    printf("time to exit timer! exit time : %s. \n\n", time_str);

    // 할 일을 다 끝낸 타이머 핸들 객체를 무효화 시킨다.
    uv_timer_stop(handle_ptr);
    // 타이머 핸들 객체가 무효화 된 다음에 해야 하는 일들을 '함수 호출'로 처리한다.
    uv_close((uv_handle_t*)handle_ptr, on_timer_close);
}

void async_nonblocking_delayed_executor(uv_loop_t* loop, int delay_in_ms, delayed_task_cb cb, void* arg)
{
    // 미룬 다음 실행할 태스크 객체를 만든다.
    delayed_task_t* task = (delayed_task_t*)malloc(sizeof(delayed_task_t));
    if (!task) return;

    // 태스크 객체의 필드들을 실행해야 하는 함수의 포인터와 인자로 초기화 한다.
    task->cb = cb;
    task->arg = arg;

    // libuv 내부에서 사용할 타이머 객체를 만든다.
    uv_timer_init(loop, &( task->timer_handle ) );

    // *** 여기가 중요하다. libuv 내에서 주어진 시간이 지난 후 콜백 함수를 실행해야 할 때,
    // libuv의 입장에서 task 객체에 접근하기 위한 포인터가 필요하기 때문이다.
    // libuv의 타이머 핸들 객체는 기본적으로 void* 타입의 데이터 포인터 필드를 포함하고 있는데,
    // 이 필드가 바로 개발 로직에 맞춰서 개발자가 설정해 줄 수 있는 공간이다.
    // 이 필드에다가 지연 시간 만료 후 실행해야 하는 함수의 포인터 등을 담고 있는 task 객체의 포인터를 집어 넣는 것이다.
    task->timer_handle.data = task;

    uv_timer_start(&( task->timer_handle ), on_timer_fire, delay_in_ms, 0); // 0 는 반복 없음을 뜻한다. 즉, 1 번만 실행함.
}






















