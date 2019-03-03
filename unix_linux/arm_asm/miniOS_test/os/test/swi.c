
#include "swi.h"

func_ptr swi_table[] = {
/* 0 */     sys_enter_critical,
/* 1 */     sys_exit_critical,
/* 2 */     
/* 10 */    
};



// turn off interrupt(In the interrupt(svc) mode)
int sys_enter_critical(void)
{
    __asm__ (
        "mrs r4, spsr\n"
        "orr r4, #0xC0\n"
        "msr r4, spsr\n"
    );
    
    return 0;
}

int sys_exit_critical(void)
{
    __asm__ (
        "mrs r4, spsr\n"
        "bic r4, #0xC0\n"
        "msr r4, spsr\n"
    );
    
    return 0;
}


int sys_sleep(void)
{
    unsigned int time;
    
    __asm__ (
        "mov %0, r0\n"
        : "=r"(time) // output
        : // input
    );
    
    // ...
}



// 





/*******************************************************************************
* API
*******************************************************************************/

// turn off interrupt(Not in interrupt mode)
// use svc mode to disable interrupt, then turn back to original mode
void OS_ENTER_CRITICAL(void)
{
    __asm__ (
        "swi %0\n"
        : // output
        : "r"(SWI_ENTER_CRITICAL) // input
    );
    
    return;
}

void OS_EXIT_CRITICAL(void)
{
    __asm__ (
        "swi %0\n"
        : // output
        : "r"(SWI_EXIT_CRITICAL) // input
    );
    
    return;
}

void sleep(unsigned int time)
{
    __asm__ (
        "mov r0, %0\n"
        "swi %1\n"
        : // output
        : "r"(time), "r"(SWI_SLEEP) // input
    );
    
    return;
}









