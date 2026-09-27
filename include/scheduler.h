#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_TASK 25

typedef void (*task_callback) (void* context);

typedef enum {
    SCHED_OK = 0,
    SCHED_ERR_FULL,
    SCHED_ERR_INVALID_ARG, //TO DO: add arg checks in the scheduler manipulation functions exposed in header.
    SCHED_ERR_INVALID_HANDLE
} Scheduler_Status; 

typedef struct{
    Scheduler_Status error;
    uint8_t handle;
} Task_Operation;


void increment_tick(void);
void scheduler_init(void);
void scheduler_run(void);


Task_Operation add_task(task_callback function, void *arguments, uint32_t run_freq, uint8_t priority);
Task_Operation activate_task(uint8_t handle);
Task_Operation deactivate_task(uint8_t handle);
Task_Operation remove_task(uint8_t handle);

#endif