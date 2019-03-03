

#ifndef SWI_H
#define SWI_H

#include "schedule.h"


#define SWI_ENTER_CRITICAL      0
#define SWI_EXIT_CRITICAL       1

#define SWI_EXIT                2
#define SWI_SLEEP               3
#define SWI_PRINT               4
#define SWI_LEDS_ON             5
#define SWI_LEDS_OFF            6
#define SWI_SET_LED             7


// function pointer
typedef int (*func_ptr)(void);
extern func_ptr swi_table[];


// function prototype
int sys_enter_critical(void);
int sys_exit_critical(void);
int sys_exit(void);
int sys_sleep(void);
int sys_print(void);
int sys_leds_on(void);
int sys_leds_off(void);
int sys_set_led(void);



#endif  // SWI_H