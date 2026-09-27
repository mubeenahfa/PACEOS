#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "scheduler.h"


typedef struct{
    task_callback callback;
    void *context;
    uint32_t period_ticks;
    uint32_t lastcall_tick;
    uint8_t priority;
    bool active;
} Task;

static Task task_table[MAX_TASK];
static bool table_isfull[MAX_TASK];
static uint8_t curr_task_count = 0;
static volatile uint32_t scheduler_tick;




void increment_tick(void){
    ++scheduler_tick;
}

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
    task->callback      = function;
    task->context       = arguments;
    task->period_ticks  = run_freq;
    task->priority      = priority;
    task->active        = 1;
    task->lastcall_tick = scheduler_tick;

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
    task_table[handle - 1].lastcall_tick = scheduler_tick;
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
    task_table[handle - 1].lastcall_tick = 0;
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
    task->callback      = NULL;
    task->context       = NULL;
    task->period_ticks  = 0;
    task->priority      = 0;
    task->active        = 0;
    task->lastcall_tick = 0;

    --curr_task_count;
    table_isfull[handle -1] = 0;

    return (Task_Operation){
            .error  = SCHED_OK,
            .handle =  0
        };

}

void scheduler_init(void){
    scheduler_tick = 0;
}


void scheduler_run(void){
    bool task_to_call = 0;
    uint8_t lowest_priority = 255;
    uint8_t next_task;
    Task *task = &task_table[0];
    for (uint8_t i = 0; i < MAX_TASK; ++i){
        if(task->active && scheduler_tick - task->lastcall_tick >= task->period_ticks && task->priority <= lowest_priority){
            lowest_priority = task->priority;
            next_task       = i;
            task_to_call    = 1;
        }
        task++;
    }
    if(task_to_call){
        task_table[next_task].lastcall_tick = scheduler_tick;
        task_table[next_task].callback(task_table[next_task].context);
    }
    

}