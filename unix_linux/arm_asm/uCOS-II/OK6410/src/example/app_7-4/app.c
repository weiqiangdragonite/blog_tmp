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
OS_MEM_DATA mem_info;
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
    
    int time = 0;
    
    // start task
    while (1) {
    
        block_ptr = OSMemGet(mem_ptr, &error);
        
        OSMemQuery(mem_ptr, &mem_info);
        
        // display character
        uart_printf("HighTask: ptr = 0x");
        uart_print_hex(*(int *) mem_info.OSFreeList);
        uart_printf(" , used = ");
        uart_print_int(mem_info.OSNUsed);
        uart_puts("");
        
        if (time++ > 6) {
            OSMemPut(mem_ptr, block_ptr);
        }
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void MidTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        block_ptr = OSMemGet(mem_ptr, &error);
        
        OSMemQuery(mem_ptr, &mem_info);
        
        // display character
        uart_printf("  MidTask: ptr = 0x");
        uart_print_hex(*(int *) mem_info.OSFreeList);
        uart_printf(" , used = ");
        uart_print_int(mem_info.OSNUsed);
        uart_puts("");
        
        OSMemPut(mem_ptr, block_ptr);
        
        // delay 2 s
        OSTimeDlyHMSM(0, 0, 2, 0);
    }
}

void LowTask(void *pdata)
{
    pdata = pdata;
    
    // start task
    while (1) {
        block_ptr = OSMemGet(mem_ptr, &error);
        
        OSMemQuery(mem_ptr, &mem_info);
        
        // display character
        uart_printf("    LowTask: ptr = 0x");
        uart_print_hex(*(int *) mem_info.OSFreeList);
        uart_printf(" , used = ");
        uart_print_int(mem_info.OSNUsed);
        uart_puts("");
        
        OSMemPut(mem_ptr, block_ptr);
        
        // delay 1 s
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}