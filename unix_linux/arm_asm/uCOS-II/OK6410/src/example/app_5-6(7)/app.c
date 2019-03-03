/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-7
*******************************************************************************/


#include "includes.h"


// task stack array
OS_STK MainTaskStk[TASK_STK_SIZE];

OS_STK HighTaskStk[TASK_STK_SIZE];
OS_STK MidTaskStk[TASK_STK_SIZE];
OS_STK LowTaskStk[TASK_STK_SIZE];


//
INT8U error;
// ex 5-6
//OS_EVENT *sem_event;

// ex 5-7
OS_EVENT *mutex_event;

int main(void)
{

    // init uC/OS-II
    OSInit();
    
    // ex 5-6
    //sem_event = OSSemCreate(1);
    
    // ex 5-7
    mutex_event = OSMutexCreate(1, &error);
    
    // create main task(prio = 0)
    OSTaskCreate(MainTask, (void *) 0, &MainTaskStk[TASK_STK_SIZE - 1], 0);
    
    // start multiple tasks management
    OSStart();
    
    return 0;
}

void MainTask(void *pdata)
{
    pdata = pdata;
    
    // init pwn clock
    init_pwm();
    
    // init stat stack
    OSStatInit();
    
    // create task
    OSTaskCreate(HighTask, (void *) 0, &HighTaskStk[TASK_STK_SIZE - 1], 2);
    OSTaskCreate(MidTask, (void *) 0, &MidTaskStk[TASK_STK_SIZE - 1], 4);
    OSTaskCreate(LowTask, (void *) 0, &LowTaskStk[TASK_STK_SIZE - 1], 6);
    
    // new line
    uart_puts("");
    
    // start task
    while (1) {
        
        // delay 3 second
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}

void HighTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        // delay 200 ms
        OSTimeDlyHMSM(0, 0, 0, 200);
        
        uart_puts("HighTask: pending semaphore");
        
        // ex 5-6
        //OSSemPend(sem_event, 0, &error);
        // ex 5-7
        OSMutexPend(mutex_event, 0, &error);
        // display character
        uart_puts("HighTask: I'm running");
        // ex 5-6
        //OSSemPost(sem_event);
        // ex 5-7
        OSMutexPost(mutex_event);
        
        // delay 200 ms
        OSTimeDlyHMSM(0, 0, 0, 200);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        uart_puts("  MidTask: I'm running");
        
        // delay 300 ms
        OSTimeDlyHMSM(0, 0, 0, 300);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        // ex 5-6
        //OSSemPend(sem_event, 0, &error);
        // ex 5-7
        OSMutexPend(mutex_event, 0, &error);
        // display character
        uart_puts("    LowTask: I'm running");
        
        OSTimeSet(0);
        uart_puts("    Before loop");
        while (OSTimeGet() < 500) {
            OS_Sched();
        }
        uart_puts("    After loop");
        // ex 5-6
        //OSSemPost(sem_event);
        // ex 5-7
        OSMutexPost(mutex_event);
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}