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



#define     SCREEN_SIZE_X       480
#define     SCREEN_SIZE_Y       272
#define     FRAME_BUFFER        0x5FA00000


#define     LINEVAL             (SCREEN_SIZE_Y - 1)
#define     HOZVAL              (SCREEN_SIZE_X - 1)

#define     COLOR_BLACK         0x000000
#define     COLOR_WHITE         0xFFFFFF
#define     COLOR_RED           0xFF0000
#define     COLOR_GREEN         0x00FF00
#define     COLOR_BLUE          0x0000FF


unsigned int ENABLE_LCD_POWER;
unsigned int ENABLE_LCD_BL;
unsigned int ENABLE_LCD_DIS;

/* function prototypes */
void initLCD(void);

void enableLCDPower(void);
void disableLCDPower(void);
void enableLCDBackLight(void);
void disableLCDBackLight(void);
void enableLCDDisplay(void);
void disableLCDDisplay(void);
void changeDisplayColor(void);

void displayColor(unsigned int color);
void TFTDotWrite(int x, int y, unsigned int color);
unsigned int TFTDotRead(int x, int y);


#endif // LCD_H