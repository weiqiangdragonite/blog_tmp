/*******************************************************************************
* File: includes.h
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-14
*******************************************************************************/


#ifndef INCLUDES_H
#define INCLUDES_H


// arm:
#include "../arm/os_cpu.h"

// kernel:
#include "../kernel/os_cfg.h"
#include "../kernel/ucos_ii.h"

// bsp:
#include "../bsp/registers.h"
#include "../bsp/led.h"
#include "../bsp/buzzer.h"
#include "../bsp/key.h"
#include "../bsp/uart.h"
#include "../bsp/time.h"
#include "../bsp/nand.h"
#include "../bsp/sdram.h"
#include "../bsp/lcd.h"
#include "../bsp/adc.h"

// app:
#include "../app/app.h"

#include "../app/atk-hc05.h"

#endif  // INCLUDES_H
