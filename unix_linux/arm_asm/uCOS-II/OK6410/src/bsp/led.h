/*******************************************************************************
* File: led.h
* 
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-14
*******************************************************************************/


#ifndef BSP_LED_H
#define BSP_LED_H

/* led mode */
#define LED_ON  0
#define LED_OFF 1

/* function prototype */
void init_led(void);

void all_leds_on(void);
void all_leds_off(void);

void set_led_1(int mode);
void set_led_2(int mode);
void set_led_3(int mode);
void set_led_4(int mode);

int get_led_1(void);
int get_led_2(void);
int get_led_3(void);
int get_led_4(void);


#endif  // BSP_LED_H
