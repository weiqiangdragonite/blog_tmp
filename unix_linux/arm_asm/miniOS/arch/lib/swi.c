#include "swi.h"
#include "led.h"
#include "schedule.h"
#include "uart.h"


func_ptr swi_table[] = {
    sys_exit,
    sys_sleep,
    sys_printf,
    sys_leds_on,
    sys_leds_off,
    sys_set_led,
    update_program,
    create_usr_process
};


int sys_exit(void)
{
    int return_code;
    
    __asm__ (
        "mov %0, r0\n"
        // output
        : "=r"(return_code)
        :
    );
    
    do_sys_exit(return_code);

    return 0;
}

void do_sys_exit(int return_code)
{
    return;
}

int sys_sleep(void)
{
    unsigned int time;
    
    __asm__ (
        "mov %0, r0\n"
        // output
        : "=r"(time)
        :
    );
    
    //uart_printf("sys_sleep: time = ");
    //uart_print_int(time);
    //uart_puts("");
    
    //time = 100;
    do_sys_sleep(time);
    
    return 0;
}

void do_sys_sleep(unsigned int time)
{
    current->state = TASK_SLEEPING;
    current->timer = time;
    
    //uart_printf("task ");
    //uart_print_int(current->pid);
    //uart_printf(" begin sleep\nsleep time = ");
    //uart_print_int(current->timer);
    //uart_puts("");
    

    schedule();
    
    return;
}

int sys_printf(void)
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



int create_usr_process(void)
{

    char *para;
    __asm__ (
        "mov %0, r0\n"
        : "=r"(para)
    );
    
    if (create_process(para) < 0) {
        uart_puts("Create process error!");
    }
    
    return 0;
}
