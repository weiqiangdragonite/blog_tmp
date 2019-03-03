/*******************************************************************************
* File: adc.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-29
*******************************************************************************/

/*

The 10-bit/12-bit CMOS ADC(Analog to Digital Converter) is a recycling type
device with 8-channel analog inputs. It converts the analog input signal into
10-bit/12-bit binary digital codes at a maximum conversion rate of 1MSPS with
5MHz A/D converter clock. A/D converter operates with on-chip sample-and-hold
function. The power down mode is supported.

Touch Screen Interface can control input pads (XP, XM, YP, and YM) to obtain
X/Y-position on the external touch screen device. Touch Screen Interface
contains three main blocks; these are touch screen pads control logic,
ADC interface logic and interrupt generation logic.

8-channel analog inputs:
AIN[3:0]
AIN4(YM)
AIN5(YP)
AIN6(XM)
AIN7(XP)


This A/D converter was designed to operate at maximum 5MHz clock, so the
conversion rate can go up to 1MSPS.

Conversion time = 1 / (5 MHz / 5 cycles) = 1 / 1 MHz = 1 us
Conversion rate = 1 s / 1 us = 1 000 000 (1 MSPS)



*/


#include "../app/includes.h"




int init_adc(int resolution, int freq, int channel, int interrupt)
{
    if (resolution == 10) {
        ADCCON &= ~(1 << 16);
    } else if (resolution == 12) {
        ADCCON |= (1 << 16);
    } else {
        return -16;
    }
    
    // A/D converter freq = HCLK / (PRSCVL + 1)
    //                    = 66 MHz / (65 + 1) = 1 MHz
    //                    = 66 MHz / (32 + 1) = 2 MHz
    //                    = 66 MHz / (21 + 1) = 3 MHz
    //                    = 66 MHz / (15 + 1) = 4.125 MHz
    //                    = 66 MHz / (14 + 1) = 4.4 MHz
    // PS: freq < PCLK / 5; maximum is 5 MHz clock
    switch (freq) {
        case 0:
            ADCCON &= ~(1 << 14);
            ADCCON |= (0xFF << 6);
            break;
        case 1:
            ADCCON |= (1 << 14);
            ADCCON &= ~(0xFF << 6);
            ADCCON |= (65 << 6);
            break;
        case 2:
            ADCCON |= (1 << 14);
            ADCCON &= ~(0xFF << 6);
            ADCCON |= (32 << 6);
            break;
        case 3:
            ADCCON |= (1 << 14);
            ADCCON &= ~(0xFF << 6);
            ADCCON |= (21 << 6);
            break;
        case 4:
            ADCCON |= (1 << 14);
            ADCCON &= ~(0xFF << 6);
            ADCCON |= (15 << 6);
            break;
        case 5:
            ADCCON |= (1 << 14);
            ADCCON &= ~(0xFF << 6);
            ADCCON |= (14 << 6);
            break;
        default:
            return -6;
    }
    
    // channel 0 ~ 3 for normal ADC conversation
    switch (channel) {
        case 0:
            ADCCON &= ~(7 << 3);
            break;
        case 1:
            ADCCON &= ~(7 << 3);
            ADCCON |= (1 << 3);
            break;
        case 2:
            ADCCON &= ~(7 << 3);
            ADCCON |= (2 << 3);
            break;
        case 3:
            ADCCON &= ~(7 << 3);
            ADCCON |= (3 << 3);
            break;
        default:
            return -3;
    }
    
    // normal operation mode
    ADCCON &= ~(1 << 2);
    // disable start by read operation
    ADCCON &= ~(1 << 1);
    
    // int
    if (interrupt == 1) {
        // INT_ADC -> VIC1 No.63
        VIC1INTENABLE |= (1 << 31);
        //if (resolution == 10) VIC1VECTADDR[23] = (unsigned int) adc_10_isr;
        //else if (resolution == 12) VIC1VECTADDR[23] = (unsigned int) adc_12_isr;
    }
    
    return 0;
}

void start_adc(void)
{
    ADCCON |= 1;
    
    // wait for conversation start
    while (ADCCON & 1);
    
    return;
}

int get_adc(int resolution)
{
    // wait for conversation end
    while (!(ADCCON & (1 << 15)));
    
    if (resolution == 10) {
        return ADCDAT0 & 0x3FF;
    } else if (resolution == 12) {
        return ADCDAT0 & 0xFFF;
    }
    
    return 0;
}

int adc_10_isr(void)
{
    //
    return ADCDAT0;
}














