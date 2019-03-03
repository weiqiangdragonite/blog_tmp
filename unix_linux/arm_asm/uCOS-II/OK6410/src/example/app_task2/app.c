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
OS_STK DeleteTaskStk[TASK_STK_SIZE];

// character position
int position = 0;

// run times
unsigned int time = 0;

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
    // create second task(prio = 5)
    OSTaskCreate(SecondTask, str_Y, &SecondTaskStk[TASK_STK_SIZE - 1], 5);
    // create third task(prio = 2)
    OSTaskCreate(DeleteTask, (void *) 0, &DeleteTaskStk[TASK_STK_SIZE - 1], 2);
    
    // new line
    uart_puts("");
    
    // start task
    while (1) {
        /*
        // for ex 3-9
        // lock the schedule
        if (time == 10) OSSchedLock();
        
        // unlock the schedule
        if (time == 40) OSSchedUnlock();
        
        ++time;
        */
    
        // display character
        uart_printf((char *) pdata);
        
        if (++position >= 30) {
            uart_printf("\n");
            position = 0;
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
    
        /*
        // for ex 3-8
        // suspend main task
        if (time == 20) OSTaskSuspend(0);
        
        // resume main task
        if (time == 40) OSTaskResume(0);
        
        ++time;
        */
        
        // for ex 3-10
        if (time == 20) {
            uart_puts("\nTask 2: I want Task 3 to delete itself!");
            // delete task
            while (OSTaskDelReq(2) != OS_ERR_TASK_NOT_EXIST) {
                
                // delay a tick
                OSTimeDly(1);
            }
        }
        
        ++time;
    
        // display character
        uart_printf((char *) pdata);
        
        if (++position >= 30) {
            uart_printf("\n");
            position = 0;
        }
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void DeleteTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
    
        // for ex 3-10
        if (OSTaskDelReq(OS_PRIO_SELF) == OS_ERR_TASK_DEL_REQ) {
            uart_puts("\nTask 3: I must delete myself!");
            
            // delay 10 second
            OSTimeDlyHMSM(0, 0, 10, 0);
            
            uart_puts("\nTask 3: I have deleted myself!");
            
            // delete myself
            OSTaskDel(OS_PRIO_SELF);
        }
    
        // display character
        uart_printf("3");
        
        if (++position >= 30) {
            uart_printf("\n");
            position = 0;
        }
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}