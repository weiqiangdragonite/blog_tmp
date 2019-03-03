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