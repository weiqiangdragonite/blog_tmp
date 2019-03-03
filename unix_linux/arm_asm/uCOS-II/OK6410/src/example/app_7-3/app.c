/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-8
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
OS_MEM *mem_ptr;
INT8U memory[50][64];
INT8U *block_ptr;


int main(void)
{

    // init uC/OS-II
    OSInit();
    
    // ex 
    mem_ptr = OSMemCreate(memory, 50, 64, &error);
    
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
    OSTaskCreate(HighTask, (void *) 0, &HighTaskStk[TASK_STK_SIZE - 1], 5);
    //OSTaskCreate(MidTask, (void *) 0, &MidTaskStk[TASK_STK_SIZE - 1], 6);
    //OSTaskCreate(LowTask, (void *) 0, &LowTaskStk[TASK_STK_SIZE - 1], 7);
    
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
    
        block_ptr = OSMemGet(mem_ptr, &error);
        
        *block_ptr = 'A';
        
        // display character
        uart_printf("HighTask: ");
        uart_puts((char *) block_ptr);
        
        OSMemPut(mem_ptr, block_ptr);
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        
    }
}