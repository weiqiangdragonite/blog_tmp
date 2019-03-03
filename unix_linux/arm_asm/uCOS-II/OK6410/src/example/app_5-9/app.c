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
OS_EVENT *queue_event;
//
#define MSG_SIZE    128
void *MsgGrp[MSG_SIZE];


int main(void)
{

    // init uC/OS-II
    OSInit();
    
    // ex 
    queue_event = OSQCreate(&MsgGrp[0], MSG_SIZE);
    
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
    
    // new line
    uart_puts("");
    
    //char *str_1 = "How many times does this str received?";
    // LIFO
    //OSQPostFront(queue_event, str_1);
    char *str_2;
    char *str_100;
    char *str_500;
    
    // start task
    while (1) {
    
        if (OSTimeGet() > 100 && OSTimeGet() < 500) {
            str_100 = "OSTime is between 100 to 500.";
            OSQPostFront(queue_event, str_100);
            str_2 = "Whick task received this str?";
            OSQPostFront(queue_event, str_2);
        }
        
        if (OSTimeGet() > 1000 && OSTimeGet() < 1500) {
            str_500 = "OSTime is between 1000 to 1500.";
            OSQPostFront(queue_event, str_500);
        }
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void HighTask(void *pdata)
{
    pdata = pdata;
    
    char *str;
    
    // start task
    while (1) {
        str = OSQPend(queue_event, 0, &error);
        // display character
        uart_printf("HighTask: ");
        uart_puts(str);
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    char *str;
    
    // start task
    while (1) {
        str = OSQPend(queue_event, 0, &error);
        uart_printf("  MidTask: ");
        uart_puts(str);
        
        // delay 1s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
    }
}