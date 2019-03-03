/******************************************************************************
* File: led.c
* 
* Description:
*	This function init the led GPIO.
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-4
******************************************************************************/


/* define the LED GPIO address */
#define		GPMCON		(*(volatile unsigned long *) (0x7F008820))
#define		GPMDAT		(*(volatile unsigned long *) (0x7F008824))
#define		GPMPUD		(*(volatile unsigned long *) (0x7F008828))


void initLED(void)
{
	// set the GPMCON low 16 bits to be - 0001 0001 0001 0001
	GPMCON = (GPMCON & 0xFFFF0000) | 0x00001111;
	
	// set the GPMPUD low 8 bits to be - 0000 0000
	GPMPUD &= 0xFFFFFF00;
	
	// set the GPMDAT low 4 bits to be - 1111 (turn off all the led)
	GPMDAT |= 0x0000000F;
	
	return;
}
