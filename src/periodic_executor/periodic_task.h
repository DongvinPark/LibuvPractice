//
// Created by dongvin on 25. 5. 3.
//

#ifndef PERIODIC_TASK_H
#define PERIODIC_TASK_H

#include <uv.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct periodic_task periodic_task_t;

typedef void (*periodic_task_cb)(void* userdata);

periodic_task_t* periodic_task_create(uv_loop_t* loop, uint64_t interval_ms);
void periodic_task_set_callback(periodic_task_t* task, periodic_task_cb cb, void* userdata);
void periodic_task_set_interval(periodic_task_t* task, uint64_t interval_ms);
void periodic_task_start(periodic_task_t* task);
void periodic_task_stop(periodic_task_t* task);
void periodic_task_destroy(periodic_task_t* task);

#endif //PERIODIC_TASK_H
