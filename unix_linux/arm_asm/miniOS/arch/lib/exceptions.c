
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
    // irq_sp = 0x5FFF5FF0 in start.s
    // push {} -> irq_sp = 0x5FFF5FF0 - 14 * 4
    // push	{lr} -> irq_sp = irq_sp - 4
    //
    /*
#define STACK_BASE          0x5FFFFFF0
#define SVC_STACK_START     STACK_BASE
#define SVC_STACK_SIZE      (32 * 1024)
#define ABT_STACK_START     (SVC_STACK_START - SVC_STACK_SIZE)
#define ABT_STACK_SIZE      (4 * 1024)
#define UND_STACK_START     (ABT_STACK_START - ABT_STACK_SIZE)
#define UND_STACK_SIZE      (4 * 1024)
#define IRQ_STACK_START     (UND_STACK_START - UND_STACK_SIZE)
#define IRQ_STACK_SIZE      (32 * 1024)
#define FIQ_STACK_START     (IRQ_STACK_START - IRQ_STACK_SIZE)
#define FIQ_STACK_SIZE      (16 * 1024)
#define SYS_STACK_START     (FIQ_STACK_START - FIQ_STACK_SIZE)
#define USR_STACK_START     SYS_STACK_START
    
    unsigned int cpsr;
    unsigned int spsr;
    unsigned int r0;
    unsigned int usr_sp;
    unsigned int usr_lr;
    __asm__ (
        "mov %2, r0\n"
        
        "mrs r0, cpsr\n"
        "mov %0, r0\n"
        
        "mrs r0, spsr\n"
        "mov %1, r0\n"
        
        "ldr r0, =0x5FFF5FF0\n"
        
        "stmia r0, {sp}^\n"
        "ldr %3, [r0]\n"
        
        "stmia r0, {lr}^\n"
        "ldr %4, [r0]\n"
 
        
        // output
        : "=r"(cpsr), "=r"(spsr), "=r"(r0), "=r"(usr_sp), "=r"(usr_lr)
        // input
        :
    );
    uart_printf("cpsr = ");
    uart_print_hex(cpsr);
    uart_printf(", spsr = ");
    uart_print_hex(spsr);
    uart_printf(", usr_sp = ");
    uart_print_hex(usr_sp);
    uart_printf(", usr_lr = ");
    uart_print_hex(usr_lr);
    uart_puts("\n");
    
    unsigned int *sp = (unsigned int *) (IRQ_STACK_START - 4);
    for (int i = 0; i < 14; ++i) {
        uart_print_hex((unsigned int) sp);
        uart_printf(" - ");
        uart_print_int(i);
        uart_printf(" : ");
        uart_print_hex(*sp--);
        uart_puts("");
    }
    
    uart_printf("\nR0 = ");
    uart_print_hex(r0);
    
    uart_puts("\n-------------------\n");
    
    // 保存上下文
    __asm__ (
        "ldr r0, =0x5FC00000\n"
        "ldr sp, =0x5FFF5FF0\n"
        
        "mrs r2, spsr\n"
        "stmia r0!, {r2}\n"
        "stmia r0!, {sp}^\n"
        "stmia r0!, {lr}^\n"
        "sub sp, sp, #0x38\n"
        "ldmia sp!, {r2-r12}\n"
        "stmia r0!, {r2-r12}\n"
        "ldmia sp!, {r2-r4}\n"
        "stmia r0!, {r2-r4}\n"
    );
    unsigned int *addr = (unsigned int *) 0x5FC00000;
    
    for (int i = 0; i < 17; ++i) {
        uart_print_hex((unsigned int) addr);
        uart_printf(" - ");
        uart_print_int(i);
        uart_printf(" : ");
        uart_print_hex(*addr++);
        uart_puts("");
    }
    
    // 清除中断
    __asm__ (
        "ldr r0, =0x7F008924\n"
        "mov r1, #0x3F\n"
        "str r1, [r0]\n"
        
        "ldr r0, =0x71200F00\n"
        "mov r1, #0\n"
        "str r1, [r0]\n"
    );
    
    // 恢复上下文
    __asm__ (
        "ldr r1, =0x5FC00000\n"
        "ldmia r1!, {r2-r4}\n"
        "msr spsr_cxsf, r2\n"
        "mrs r2, cpsr\n"
        "msr cpsr_c, #0xdf\n"
        "mov sp, r3\n"
        "mov lr, r4\n"
        "msr cpsr_cxsf, r2\n"
        "ldmia r1, {r0-r12, pc}^\n"
    );
    
    
    
    
    while (1);
    */
    
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