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
static bool table_isfull[MAX_TASK];
static uint8_t curr_task_count = 0;

typedef struct{
    Scheduler_Status error;
    uint8_t handle;
} Task_Operation;


Task_Operation add_task(task_callback function, void *arguments, uint32_t run_freq, uint8_t priority){

    uint8_t first_empty_slot;
    bool slot_found = 0; 
    for (first_empty_slot = 0; first_empty_slot < MAX_TASK; ++first_empty_slot){
        if (table_isfull[first_empty_slot] == 0){
            slot_found =1;
            break;
        }
    }
    
    if (!slot_found){
        return (Task_Operation){
            .error  = SCHED_ERR_FULL,
            .handle = 0
        };
    }


    Task *task = &task_table[first_empty_slot];
    task->callback     = function;
    task->context      = arguments;
    task->period_ticks = run_freq;
    task->priority     = priority;
    task->active       = 1;

    ++curr_task_count;
    table_isfull[first_empty_slot] = 1;

    return (Task_Operation){
            .error  = SCHED_OK,
            .handle = ++first_empty_slot
        };

}

Task_Operation activate_task(uint8_t handle){
    if (handle == 0 || handle > MAX_TASK || !table_isfull[handle - 1]){
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
    if (handle == 0 || handle > MAX_TASK || !table_isfull[handle - 1]){
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

Task_Operation remove_task(uint8_t handle){
    
    if (handle == 0 || handle > MAX_TASK || !table_isfull[handle - 1]){
        return (Task_Operation){
            .error  = SCHED_ERR_INVALID_HANDLE,
            .handle =  0
        };
    }

    Task *task = &task_table[handle - 1];
    task->callback     = NULL;
    task->context      = NULL;
    task->period_ticks = 0;
    task->priority     = 0;
    task->active       = 0;

    --curr_task_count;
    table_isfull[handle -1] = 0;

    return (Task_Operation){
            .error  = SCHED_OK,
            .handle =  0
        };

}
