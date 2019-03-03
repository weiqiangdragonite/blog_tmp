
#include "uart.h"
#include "registers.h"
#include "swi.h"


void handle_und(void)
{
    //
    uart_puts("\nUndefined Instruction.");
    while (1);
    
    return;
}

void handle_swi()
{
    //
    //uart_puts("\nSoftware Interrupt.");
    //uart_printf("num = ");
    //uart_print_int(num);
    //uart_puts("");
    
    // r0: num
    //__asm__ (
     //   "ldr r4, =%0\n"
     //   ""
    //    "ldr pc, [r4, r0]\n"
        
    //);
    
    //swi_table[num]();
    
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
    //uart_puts("\nIRQ");
    
    // 函数指针
	void (*the_isr)(void);
	the_isr = (void *) VIC0ADDRESS;
	(*the_isr)();
    
    return;
}

void handle_fiq(void)
{
    //
    uart_puts("\nFIQ");
    while (1);
    
    return;
}