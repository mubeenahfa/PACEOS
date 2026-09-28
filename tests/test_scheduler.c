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
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    scheduler_init();
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
    Task_Operation task_zero = add_task(test_callback,&val,5,1);
    scheduler_init();
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
    Task_Operation task_zero = add_task(test_callback,&val,5,1);

    Task_Operation task_one = add_task(test_callback,&vald,5,5);
    scheduler_init();
    for (uint8_t counter = 0; counter < 6; ++counter){
        printf("fucking %d \n", id);
        scheduler_run();
        increment_tick();
    }
    printf("fucking %d", id);
    return id == 0;

}



//TEST4: if deactivated, does it stop running

//TEST5: if reactivated, does its period restart properly

//TEST6: if I remove one, does its slot become reusable

//TEST7: what happens when I fill in all slots and then try to add another
int main(void){
    //printf("Test one: %s\n", Test_one() ? "PASS" : "FAIL");
    printf("Test two: %s\n", Test_two() ? "PASS" : "FAIL");
    printf("Test three: %s\n", Test_three() ? "PASS" : "FAIL");
    return 0;
}