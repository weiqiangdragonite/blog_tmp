
#include "uart.h"
#include "registers.h"


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

void handle_pabt(unsigned int lr)
{
    //
    uart_puts("\nPrefetch Abort");
    uart_printf("PC = 0x");
    uart_print_hex(lr);
    uart_puts("");
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
	the_isr = (void *) VIC0ADDRESS;
	(*the_isr)();
    
    return;
}

void OS_CPU_FIQ_ISR_Handler(void)
{
    uart_puts("\nFIQ");
    while (1);
    
    return;
}