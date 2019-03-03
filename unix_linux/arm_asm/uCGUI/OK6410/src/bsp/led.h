

#ifndef LED_H
#define LED_H

#include "registers.h"

/* function prototype */
void init_led(void);
void all_leds_on(void);
void all_leds_off(void);
void set_led_1(int mode);
void set_led_2(int mode);
void set_led_3(int mode);
void set_led_4(int mode);


#endif  // LED_H