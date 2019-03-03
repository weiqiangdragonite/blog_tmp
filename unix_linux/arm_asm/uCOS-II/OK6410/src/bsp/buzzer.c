/*******************************************************************************
* File: buzzer.c
* 
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-14
*******************************************************************************/


#include "../app/includes.h"


void init_buzzer(void)
{
    // buzzer use GPF15
    // set GPFCON[31:30] to be output - 01
    GPFCON &= ~(3 << 30);
    GPFCON |= (1 << 30);
    
    // set GPFPUD[31:30] to disable pull-up/down - 00
    GPFPUD &= ~(3 << 30);
    
    // GPFDAT - high level: turn on buzzer; low level: turn off buzzer

    return;
}

void turn_on_buzzer(void)
{
    // set GPFDAT[15] to high level
    GPFDAT |= (1 << 15);
    
    return;
}

void turn_off_buzzer(void)
{
    // set GPFDAT[15] to low level
    GPFDAT &= ~(1 << 15);
    
    return;
}