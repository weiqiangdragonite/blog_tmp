



#define SWI_exit        0
#define SWI_sleep       1
#define SWI_printf      2
#define SWI_leds_on     3
#define SWI_leds_off    4
#define SWI_set_led     5


int _exit(int return_code);
int sleep(unsigned int time);
void leds_on(void);
void leds_off(void);