#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "scheduler.h"
uint16_t id = 0;


void test_callback(void *context){
    id = *(int *)context;
}



//TEST 1: if task period = 5, is it run at 5?
bool Test_one(){
    int val = 0;
    scheduler_init();
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    for (uint8_t counter = 0; counter < 7; ++counter){
        scheduler_run();
        increment_tick();
        val++;
    }
    return id == 5;

}
//TEST2: after 5 does it run at 10 next?
bool Test_two(){
    int val = 0;
    scheduler_init();
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    for (uint8_t counter = 0; counter < 12; ++counter){
        scheduler_run();
        increment_tick();
        val++;
    }
    return id == 10;

}

//TEST3: if two tasks are active same time, is priority working currently
bool Test_three(){
    id = 99;
    int val = 0;
    int vald = 1;
    //printf(" %d \n", id);
    scheduler_init();
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    Task_Operation task_one = add_task(test_callback,&vald,5,5);
    for (uint8_t counter = 0; counter < 6; ++counter){
        scheduler_run();
        increment_tick();
    }
    return id == 0;

}



//TEST4: if deactivated, does it stop running

bool Test_four(){
    id = 99;
    int val = 0;
    int vald = 1;
    //printf(" %d \n", id);
    scheduler_init();
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    Task_Operation task_one = add_task(test_callback,&vald,5,5);
    for (uint8_t counter = 0; counter < 6; ++counter){
        //printf("fucking %d \n", id);
        scheduler_run();
        increment_tick();
        if (counter == 2){
            task_zero = deactivate_task(task_zero.handle);
        }
    }
    //printf("fucking %d", id);
    return id == 1;

}

//TEST5: if reactivated, does its period restart properly
bool Test_five(){
    id = 99;
    int val = 0;
    int vald = 1;
    //printf(" %d \n", id);
    scheduler_init();
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    Task_Operation task_one = add_task(test_callback,&vald,5,5);
    for (uint8_t counter = 0; counter < 6; ++counter){
        //printf("fucking %d \n", id);
        scheduler_run();
        increment_tick();
        if (counter == 2){
            task_zero = deactivate_task(task_zero.handle);
        }
        if (counter == 4){
            task_zero = activate_task(task_zero.handle);
        }
    }
    //printf("fucking %d", id);
    return id == 1;

}

//TEST6: if I remove one, does its slot become reusable
bool Test_six(){
    id = 99;
    int val = 0;
    int vald = 1;
    //printf(" %d \n", id);
    scheduler_init();
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    Task_Operation task_one = add_task(test_callback,&val,5,1);
    Task_Operation task_two = add_task(test_callback,&vald,5,5);
    task_zero  = remove_task(task_zero.handle);
    Task_Operation task_three = add_task(test_callback,&vald,5,5);
    
    
    return task_three.handle == 1;

}

//TEST7: what happens when I fill in all slots and then try to add another
bool Test_seven(){
    id = 99;
    int val = 0;
    int vald = 1;
    //printf(" %d \n", id);
    scheduler_init();
    Task_Operation tasks[MAX_TASK];
    for (uint8_t i = 0; i < MAX_TASK; ++i){
        tasks[i] = add_task(test_callback,&vald,5,5);
    }

    Task_Operation task_extra = add_task(test_callback,&vald,5,5);
    
    
    return task_extra.error == SCHED_ERR_FULL;

}



int main(void){
    printf("Test one: %s\n", Test_one() ? "PASS" : "FAIL");
    printf("Test two: %s\n", Test_two() ? "PASS" : "FAIL");
    printf("Test three: %s\n", Test_three() ? "PASS" : "FAIL");
    printf("Test four: %s\n", Test_four() ? "PASS" : "FAIL");
    printf("Test five: %s\n", Test_five() ? "PASS" : "FAIL");
    printf("Test six: %s\n", Test_six() ? "PASS" : "FAIL");
    printf("Test seven: %s\n", Test_seven() ? "PASS" : "FAIL");
    return 0;
}