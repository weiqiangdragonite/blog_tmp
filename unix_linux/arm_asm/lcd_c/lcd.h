/*******************************************************************************
* File: lcd.h
* 
* Description:
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-16
*******************************************************************************/

#ifndef LCD_H
#define LCD_H


/* include the registers address */
#include "registers.h"


/* function prototypes */
void initLCD(void);

void enableLCDPower(void);
void disableLCDPower(void);
void enableLCDBackLight(void);
void disableLCDBackLight(void);
void enableLCDDisplay(void);
void disableLCDDisplay(void);
void changeDisplayColor(void);


#endif // LCD_H