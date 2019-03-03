

#ifndef SWI_H
#define SWI_H


#define SWI_exit        0
#define SWI_sleep       1
#define SWI_printf      2
#define SWI_leds_on     3
#define SWI_leds_off    4
#define SWI_set_led     5


// function pointer
typedef int (*func_ptr)(void);
extern func_ptr swi_table[];


int sys_exit(void);
int sys_sleep(void);
int sys_printf(void);
int sys_leds_on(void);
int sys_leds_off(void);
int sys_set_led(void);
extern int update_program(void);

int create_usr_process(void);
extern int create_process(char *parameters);


void do_sys_exit(int return_code);
void do_sys_sleep(unsigned int time);




#endif  // SWI_H