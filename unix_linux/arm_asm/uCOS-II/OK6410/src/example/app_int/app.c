/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-6
*******************************************************************************/


#include "includes.h"


// task stack array
OS_STK TaskStartStk[TASK_STK_SIZE];
OS_STK SecondTaskStk[TASK_STK_SIZE];

// character position
int x = 0;

int main(void)
{

    // init uC/OS-II
    OSInit();

    // define display str
    char *str_M = "M";
    
    // create main task(prio = 0)
    OSTaskCreate(TaskStart, str_M, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    
    // start multiple tasks management
    OSStart();
    
    return 0;
}

void TaskStart(void *pdata)
{
    pdata = pdata;
    
    // init pwn clock
    init_pwm();
    
    // init stat stack
    OSStatInit();
    
    char *str_Y = "Y";
    // create second task(prio = 1)
    OSTaskCreate(SecondTask, str_Y, &SecondTaskStk[TASK_STK_SIZE - 1], 1);
    
    // new line
    uart_puts("");
    
    // start task
    while (1) {
        // display character
        uart_printf((char *) pdata);
        
        if (++x >= 30) {
            uart_printf("\n");
            x = 0;
        }
        
        // delay 3 second
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}

void SecondTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        // display character
        uart_printf((char *) pdata);
        
        if (++x >= 30) {
            uart_printf("\n");
            x = 0;
        }
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}