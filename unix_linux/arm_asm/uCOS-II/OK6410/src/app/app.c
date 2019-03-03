/*******************************************************************************
* File: app.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-24
*******************************************************************************/


#include "includes.h"


// task stack array
OS_STK MainTaskStk[TASK_STK_SIZE];

OS_STK HighTaskStk[TASK_STK_SIZE];
OS_STK MidTaskStk[TASK_STK_SIZE];
OS_STK LowTaskStk[TASK_STK_SIZE];


int time = 1;
int screen_width = 480;
int screen_height = 272;


int main(void)
{
    unsigned int addr = (unsigned int) &MainTaskStk[99];
    uart_printf("In the main\n");
    uart_printf("stack[99] addr: ");
    uart_print_addr(addr);
    uart_printf("\n");
    addr = (unsigned int) &MainTaskStk[0];
    uart_printf("stack[0] addr: ");
    uart_print_addr(addr);
    uart_printf("\n");

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
    // init pwn clock
    init_pwm();
    
    init_led();
    init_buzzer();
    
    // init hc05
    init_hc05();
    
    //
    //init_timer_1();
    //start_timer_1();
    
    // lcd
    config_lcd(screen_width, screen_height, SCREEN_HOIZONTAL);
    init_lcd();
    lcd_display_string(1, 21, COLOR_RED, COLOR_WHITE, "OK6410 is Rocked!");
    
    init_adc(10, 1, 2, 0);
    
    
    
    pdata = pdata;
    
    // init stat stack
    OSStatInit();
    
    // create task
    OSTaskCreate(HighTask, (void *) 0, &HighTaskStk[TASK_STK_SIZE - 1], 5);
    //OSTaskCreate(MidTask, (void *) 0, &MidTaskStk[TASK_STK_SIZE - 1], 6);
    //OSTaskCreate(LowTask, (void *) 0, &LowTaskStk[TASK_STK_SIZE - 1], 7);
    
    
    // new line
    uart_puts("");
    
    char ch = 'A';
    
    // start task
    while (1) {
        //uart_printf("\nin the main task");
        
        if (hc05_state()) {
            if (ch > 'Z') ch = 'A';
            set_led_2(LED_ON);
            hc05_putc(ch);
            ++ch;
        } else {
            set_led_2(LED_OFF);
        }
        
        start_adc();
        
    
        // delay 1 s
        //OSTimeDly()
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void HighTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        //uart_printf("\nin the high task");
        
        if (time) {
            set_led_1(LED_ON);
            time = 0;
        } else {
            set_led_1(LED_OFF);
            time = 1;
        }
        
        uart_printf("value = ");
        int value = get_adc(10);
        uart_print_int(value);
        uart_puts("");
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        
        // delay 2 s
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

