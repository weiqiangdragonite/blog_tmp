/*******************************************************************************
* File: app.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-6
*******************************************************************************/


#include "includes.h"


// task stack array
OS_STK  MainTaskStk[TASK_STK_SIZE];
OS_STK  SecondTaskStk[TASK_STK_SIZE];


int main(void)
{
    // turn off all led
    all_leds_off();

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
    
    BOOLEAN led_state;
    led_state = FALSE;
    
    // init pwn clock
    init_pwm();
    
    // create second task(prio = 1)
    OSTaskCreate(SecondTask, (void *) 0, &SecondTaskStk[TASK_STK_SIZE - 1], 1);
    
    while (1) {
    
        if (led_state == FALSE) {
            led_state = TRUE;
            set_led_1(0);
        } else {
            led_state = FALSE;
            set_led_1(1);
        }
        
        // delay 1 second
        OSTimeDlyHMSM(0, 0, 1, 0);
        // OSTimeDly(ticks) : delay n clock ticks
    }
}


void SecondTask(void *pdata)
{
    pdata = pdata;
    
    BOOLEAN led_state;
    led_state = FALSE;
    
    while (1) {
    
        if (led_state == FALSE) {
            led_state = TRUE;
            set_led_2(0);
        } else {
            led_state = FALSE;
            set_led_2(1);
        }
        
        // delay 3 seconds
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}
