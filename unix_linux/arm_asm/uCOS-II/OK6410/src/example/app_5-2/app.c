/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-7
*******************************************************************************/


#include "includes.h"


// task stack array
OS_STK MainTaskStk[TASK_STK_SIZE];
OS_STK SecondTaskStk[TASK_STK_SIZE];

// character position
//int position = 0;
//
char *str;
int mutex;

int main(void)
{

    // init uC/OS-II
    OSInit();
    
    mutex = 1;
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
    
    // create second task(prio = 1)
    OSTaskCreate(SecondTask, (void *) 0, &SecondTaskStk[TASK_STK_SIZE - 1], 1);
    
    // new line
    uart_puts("");
    
    // start task
    while (1) {
        // display character
        
        uart_printf("  MainTask: ");
        if (mutex) {
            --mutex;
            str = "  [Main Task]  ";
            uart_puts(str);
            ++mutex;
        }
        
        // delay 20 ticks
        OSTimeDly(20);
    }
}

void SecondTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        // display character
        uart_printf("  SecondTask: ");
        if (mutex) {
            --mutex;
            
            str = "  [Second Task]  ";
            uart_printf(str);
            
            OSTimeSet(0);
            
            uart_puts("\nbefore loop");
            while (OSTime < 500) {
                if (OSTime % 80 == 0) uart_puts(str);
            }
            uart_puts("\nafter lopp");
            
            ++mutex;
        }
        
        // delay 10 ticks
        OSTimeDly(10);
    }
}