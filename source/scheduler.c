#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "scheduler.h"


typedef void (*task_callback) (void* context);

typedef struct{
    task_callback callback;
    void *context;
    uint32_t period_ticks;
    uint8_t priority;
    bool active;
} Task;

typedef enum {
    SCHED_OK = 0,
    SCHED_ERR_FULL,
    SCHED_ERR_INVALID_ARG, //TO DO: add arg checks in the scheduler manipulation functions exposed in header.
    SCHED_ERR_INVALID_HANDLE
} Scheduler_Status; 

static Task task_table[MAX_TASK];
static uint8_t curr_task_count = 0;

typedef struct{
    Scheduler_Status error;
    uint8_t handle;
} Task_Operation;


Task_Operation add_task(task_callback function, void *arguments, uint32_t run_freq, uint8_t priority){

    if (curr_task_count >= MAX_TASK){
        return (Task_Operation){
            .error  = SCHED_ERR_FULL,
            .handle = 0
        };
    }

    Task *task = &task_table[curr_task_count];
    task->callback     = function;
    task->context      = arguments;
    task->period_ticks = run_freq;
    task->priority     = priority;
    task->active       = 1;

    ++curr_task_count;

    return (Task_Operation){
            .error  = SCHED_OK,
            .handle = curr_task_count
        };

}

Task_Operation activate_task(uint8_t handle){
    if (handle > MAX_TASK || handle == 0 || handle > curr_task_count){
        return (Task_Operation){
            .error  = SCHED_ERR_INVALID_HANDLE,
            .handle =  0
        };
    }
    task_table[handle - 1].active = 1;
    return (Task_Operation){
            .error  = SCHED_OK,
            .handle =  handle
        };

    }

Task_Operation deactivate_task(uint8_t handle){
    if (handle > MAX_TASK || handle == 0 || handle > curr_task_count){
        return (Task_Operation){
            .error  = SCHED_ERR_INVALID_HANDLE,
            .handle =  0
        };
    }
    task_table[handle - 1].active = 0;
    return (Task_Operation){
            .error  = SCHED_OK,
            .handle =  handle
        };

    }

