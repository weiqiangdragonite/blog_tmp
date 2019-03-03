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
void *ptr_msg;

int main(void)
{

    // init uC/OS-II
    OSInit();
    
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
        // display character
        uart_printf("  FirstTask  ");
        if (ptr_msg != (void *) 0) {
            uart_printf(": ");
            uart_printf(ptr_msg);
            ptr_msg = (void *) 0;
        }
        uart_puts("");
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void SecondTask(void *pdata)
{
    pdata = pdata;
    
    char *msg = "MSG from SecondTask!";
    
    // start task
    while (1) {
        // display character
        uart_printf("  SecondTask  ");
        if (OSTimeGet() > 500 && ptr_msg == (void *) 0) {
            ptr_msg = msg;
            uart_printf(": I have send an msg.");
        }
        uart_puts("");
        
        // delay 2 second
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}