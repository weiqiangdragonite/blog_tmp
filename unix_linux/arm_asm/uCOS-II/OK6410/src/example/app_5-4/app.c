/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-7
*******************************************************************************/


#include "includes.h"


// task stack array
OS_STK MainTaskStk[TASK_STK_SIZE];
OS_STK FirstTaskStk[TASK_STK_SIZE];
OS_STK SecondTaskStk[TASK_STK_SIZE];

// character position
//int position = 0;
//
char *str;
//
INT8U error;
//
OS_EVENT *sem_event;

int main(void)
{

    // init uC/OS-II
    OSInit();
    
    //
    sem_event = OSSemCreate(1);
    
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
    OSTaskCreate(FirstTask, (void *) 0, &FirstTaskStk[TASK_STK_SIZE - 1], 1);
    OSTaskCreate(SecondTask, (void *) 0, &SecondTaskStk[TASK_STK_SIZE - 1], 2);
    
    // new line
    uart_puts("");
    
    // start task
    while (1) {
        
        // delay 3 second
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}

void FirstTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        //
        OSSemPend(sem_event, 0, &error);
        // display character
        uart_printf("  FirstTask: ");
        function();
        uart_puts("");
        OSSemPost(sem_event);
        
        // delay 1 second
        //OSTimeDlyHMSM(0, 0, 1, 0);
        OSTimeDly(20);
    }
}

void SecondTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        //
        OSSemPend(sem_event, 0, &error);
        // display character
        uart_printf("  SecondTask: ");
        function();
        uart_puts("");
        
        OSTimeSet(0);
        while (OSTime < 500) {
            if (OSTime % 80 == 0) uart_puts("in the loop");
        }
        OSSemPost(sem_event);
        
        // delay 2 second
        //OSTimeDlyHMSM(0, 0, 2, 0);
        OSTimeDly(10);
    }
}

void function(void)
{
    uart_printf("call function()");
    
    return;
}