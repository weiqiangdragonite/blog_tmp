/*******************************************************************************
* File: led.c
* 
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-14
*******************************************************************************/


#include "../app/includes.h"


void init_led(void)
{
    // led 1 ~ led 4 use GPM0 ~ GPM3
	// set the GPMCON[15:0]to be output - 0001 0001 0001 0001
	GPMCON &= ~0xFFFF;
	GPMCON |= 0x1111;
	
	// set the GPMPUD[7:0] to disable pull-up/down - 0000 0000
	GPMPUD &= ~0xFF;
	
    // high level: turn off led; low level: turn on led
	// set the GPMDAT[3:0] to high level(turn off all the led) - 1111
	GPMDAT |= 0xF;
	
	return;
}

void all_leds_on(void)
{
    // turn on all leds
    GPMDAT &= ~0xF;
    
    return;
}

void all_leds_off(void)
{
    // turn off all leds
    GPMDAT |= 0xF;
    
    return;
}

void set_led_1(int mode)
{
    if (mode == LED_OFF) {
        // turn off led 1
        GPMDAT |= 1;
    } else if (mode == LED_ON) {
        // turn on led 1
        GPMDAT &= ~1;
    }
    
    return;
}

void set_led_2(int mode)
{
    if (mode == LED_OFF) {
        // turn off led 2
        GPMDAT |= 2;
    } else if (mode == LED_ON) {
        // turn on led 2
        GPMDAT &= ~2;
    }
    
    return;
}

void set_led_3(int mode)
{
    if (mode == LED_OFF) {
        // turn off led 3
        GPMDAT |= 4;
    } else if (mode == LED_ON) {
        // turn on led 3
        GPMDAT &= ~4;
    }
    
    return;
}

void set_led_4(int mode)
{
    if (mode == LED_OFF) {
        // turn off led 4
        GPMDAT |= 8;
    } else if (mode == LED_ON) {
        // turn on led 4
        GPMDAT &= ~8;
    }
    
    return;
}

int get_led_1(void)
{
    unsigned int mode = GPMDAT & 1;
    
    if (mode) return LED_OFF;
    
    return LED_ON;
}

int get_led_2(void)
{
    unsigned int mode = GPMDAT & 2;
    
    if (mode) return LED_OFF;
    
    return LED_ON;
}

int get_led_3(void)
{
    unsigned int mode = GPMDAT & 4;
    
    if (mode) return LED_OFF;
    
    return LED_ON;
}

int get_led_4(void)
{
    unsigned int mode = GPMDAT & 8;
    
    if (mode) return LED_OFF;
    
    return LED_ON;
}
