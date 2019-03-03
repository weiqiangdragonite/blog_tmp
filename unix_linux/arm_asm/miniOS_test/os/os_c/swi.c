#include "swi.h"
#include "led.h"
#include "uart.h"





// <-- line number is swi number
func_ptr swi_table[] = {
    sys_enter_critical,
    sys_exit_critical,
    sys_exit,
    sys_sleep,
    sys_print,
    sys_leds_on,
    sys_leds_off,
    sys_set_led
};




/*
 *
 *
 */

int sys_enter_critical(void)
{
    __asm__ (
        "mrs r4, spsr\n"
        "orr r4, #0xC0\n"
        "msr spsr, r4\n"
    );
    
    return 0;
}

int sys_exit_critical(void)
{
    __asm__ (
        "mrs r4, spsr\n"
        "bic r4, #0xC0\n"
        "msr spsr, r4\n"
    );
    
    return 0;
}




int sys_exit(void)
{
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
    current->state = TASK_SLEEPING;
    current->timer = time;
    
    schedule();
    
    return 0;
}

int sys_print(void)
{
    return 0;
}

int sys_leds_on(void)
{
    all_leds_on();
    
    return 0;
}

int sys_leds_off(void)
{
    all_leds_off();
    
    return 0;
}

int sys_set_led(void)
{
    return 0;
}



