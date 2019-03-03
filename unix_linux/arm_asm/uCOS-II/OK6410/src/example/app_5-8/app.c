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
// ex 
OS_EVENT *mbox_event;
//
unsigned int run_time = 0;


int main(void)
{

    // init uC/OS-II
    OSInit();
    
    // ex 
    mbox_event = OSMboxCreate((void *) 0);
    
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
    
    void *msg = &run_time;
    
    // start task
    while (1) {
        ++run_time;
        OSMboxPost(mbox_event, msg);
        // display character
        uart_puts("HighTask: I'm running");
        
        // delay 200 ms
        OSTimeDlyHMSM(0, 0, 0, 200);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    void *msg;
    
    // start task
    while (1) {
        msg = OSMboxPend(mbox_event, 100, &error);
        uart_printf("  MidTask: HighTask run times = ");
        uart_print_int(*(int *) msg);
        uart_puts("");
        
        // delay 300 ms
        OSTimeDlyHMSM(0, 0, 0, 300);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
    }
}