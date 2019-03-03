#include "lib.h"

int _exit(int return_code)
{
    int num;
    
    __asm__ (
        "mov r0, %0\n"
        "swi 0\n"
        "mov %0, r0\n"
        // output
        : "=r"(num)
        // input
        : "r"(return_code)
    );
    
    return num;
}

int sleep(unsigned int time)
{
    __asm__ (
        "mov r0, %0\n"
        "swi 1\n"
        // output
        :
        // input
        : "r"(time)
    );
    
    return 0;
}

void leds_on(void)
{
    __asm__ (
        "swi 3\n"
    );
    
    return;
}

void leds_off(void)
{
    __asm__ (
        "swi 4\n"
    );
    
    return;
}