

#include "led.h"

void init_led(void)
{
	// set the GPMCON low 16 bits to be output - 0001 0001 0001 0001
	GPMCON &= ~0xFFFF;
	GPMCON |= 0x1111;
	
	// set the GPMPUD low 8 bits to be - 0000 0000
	GPMPUD &= ~0xFF;
	
	// set the GPMDAT low 4 bits to be - 1111 (turn off all the led)
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
    if (mode == 0) {
        // turn on led 1
        GPMDAT &= ~1;
    } else {
        // turn off led 1
        GPMDAT |= 1;
    }
    
    return;
}

void set_led_2(int mode)
{
    if (mode == 0) {
        // turn on led 2
        GPMDAT &= ~2;
    } else {
        // turn off led 2
        GPMDAT |= 2;
    }
    
    return;
}

void set_led_3(int mode)
{
    if (mode == 0) {
        // turn on led 3
        GPMDAT &= ~4;
    } else {
        // turn off led1
        GPMDAT |= 4;
    }
    
    return;
}

void set_led_4(int mode)
{
    if (mode == 0) {
        // turn on led 4
        GPMDAT &= ~8;
    } else {
        // turn off led 4
        GPMDAT |= 8;
    }
    
    return;
}