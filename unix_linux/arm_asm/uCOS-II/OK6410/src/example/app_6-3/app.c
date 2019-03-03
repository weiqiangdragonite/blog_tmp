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
OS_FLAG_GRP *sem_flag;
OS_FLAGS flags;


int main(void)
{

    // init uC/OS-II
    OSInit();
    
    // ex 
    sem_flag = OSFlagCreate((OS_FLAGS) 0, &error);
    
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
    OSTaskCreate(MidTask, (void *) 0, &MidTaskStk[TASK_STK_SIZE - 1], 6);
    OSTaskCreate(LowTask, (void *) 0, &LowTaskStk[TASK_STK_SIZE - 1], 7);
    
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
    
    char *str;
    
    // start task
    while (1) {
    
        flags = OSFlagQuery(sem_flag, &error);
        
        switch (flags) {
            case 0:
                str = "NO active.";
                break;
            case 0x1:
                str = "Bit 0 is active.";
                break;
            case 0x2:
                str = "Bit 1 is active.";
                break;
            case 0x3:
                str = "Both bit 0 and 1 is active.";
                break;
            default:
                str = "ERROR";
                break;
        }
        
        OSFlagPost(sem_flag, (OS_FLAGS) 3, OS_FLAG_CLR, &error);
        
        // display character
        uart_printf("HighTask: ");
        uart_puts(str);
        
        // delay 2 s
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        uart_puts("  MidTask: I'm running.");
        
        // delay 8 s
        OSTimeDlyHMSM(0, 0, 8, 0);
        
        OSFlagPost(sem_flag, (OS_FLAGS) 2, OS_FLAG_SET, &error);
        
        // delay 2 s
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        uart_puts("    LowTask: I'm running.");
        
        OSFlagPost(sem_flag, (OS_FLAGS) 1, OS_FLAG_SET, &error);
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}