/*******************************************************************************
* File: exceptions.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-15
*******************************************************************************/


#include "../app/includes.h"


void handle_und(void)
{
    //
    uart_puts("\nUndefined Instruction");
    while (1);
    
    return;
}

void handle_swi()
{

    
    return;
}

void handle_pabt(void)
{
    //
    uart_puts("\nPrefetch Abort");
    while (1);
    
    return;
}

void handle_dabt(void)
{
    //
    uart_puts("\nData Abort");
    while (1);
    
    return;
}

void handle_irq(void)
{
    //
    uart_puts("\nIRQ");
    while (1);

    return;
}

void handle_fiq(void)
{
    //
    uart_puts("\nFIQ");
    while (1);
    
    return;
}




void OS_CPU_IRQ_ISR_Handler(void)
{
    void (*the_isr)(void);
    
    if (VIC0IRQSTATUS) {
	    the_isr = (void *) VIC0ADDRESS;
	} else if (VIC1IRQSTATUS) {
	    the_isr = (void *) VIC1ADDRESS;
	}
	
	(*the_isr)();
    
    return;
}

void OS_CPU_FIQ_ISR_Handler(void)
{
    uart_puts("\nFIQ");
    while (1);
    
    return;
}
