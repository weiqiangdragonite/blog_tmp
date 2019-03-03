/*******************************************************************************
* File: lcd.c
* 
* Description:
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-16
*******************************************************************************/


#include "lcd.h"

#define     FRAME_BUFFER        0x55000000
#define     LINEVAL             271
#define     HOZVAL              479


void initLCD(void)
{
    /** 1. 设置LCD的相关GPIO引脚 */
    // VD0 ~ VD23的引脚为GPI0 ~ GPI15，GPJ0 ~ GPJ7
    // LDVEN、LVSYNC、LHSYNC、LVCLK的引脚为GPJ8 ~ GPJ11
    GPICON = 0xAAAAAAAA;
    GPJCON = 0xAAAAAA;
    
    // GPF14用作背光使能信号，设为输出模式
    // GPE0用作LCD的电源使能信号，设为输出模式
    GPFCON &= ~(3 << 28);
    GPFCON |= (1 << 28);
    GPECON &= ~(0xF);
    GPECON |= 1;
    
    /** 2. 初始化LCD控制器 */
    // PROGRAMMER'S MODEL
    // 设置MIFPCON的SEL_BYPASS[3]为Normal mode
    MIFPCON &= ~(1 << 3);
    
    // 设置SPCON的LCD_SEL[1:0]为RGB I/F Style
    SPCON = (SPCON & (~3)) | 1;
    
    // 设置VIDCON0控制器，设置图像输出格式等属性
    VIDCON0 &= ~((3 << 26) | (3 << 17) | (0xFF << 6) | 0x1F);
    VIDCON0 |= ((13 << 6) | (1 << 4));
    
    // 设置VIDCON1控制器，设置HSYNC、VSYNC、VCLK、VDEN
    VIDCON1 &= ~(0xF << 4);
    VIDCON1 |= (0x7 << 4);
    
    // 设置VIDTCON控制器的时间参数，行列数
    VIDTCON0 = (1 << 16) | (1 << 8) | 9;
    VIDTCON1 = (1 << 16) | (1 << 8) | 40;
    VIDTCON2 = (LINEVAL << 11) | HOZVAL;
    
    // 设置WINCON0的输出像素格式
    WINCON0 &= ~(0xF << 2);
    WINCON0 |= (0xB << 2);
    
    // 设置左上角和右下角坐标
    VIDOSD0A = 0;
    VIDOSD0B = (HOZVAL << 11) | LINEVAL;
    
    // 设置图像大小
    VIDOSD0C = (HOZVAL + 1) * (LINEVAL + 1);
    
    // 设置显存起始地址
    VIDW00ADD0B0 = FRAME_BUFFER;
    // 设置显存结束地址
    VIDW00ADD1B0 = (HOZVAL + 1) * 4 * (LINEVAL + 1);
    
    return;
}

void enableLCDPower(void)
{
    // 输出高电平，使能电源
    // 看不出有作用
    GPEDAT |= 1;
    
    return;
}

void disableLCDPower(void)
{
    // 输出低电平
    // 看不出有作用
    GPEDAT &= ~1;
    
    return;
}

void enableLCDBackLight(void)
{
    GPFDAT |= (1<<14);
    
    return;
}

void disableLCDBackLight(void)
{
    GPFDAT &= ~(1<<14);
    
    return;
}

void enableLCDDisplay(void)
{
    VIDCON0 |= 3;
	WINCON0 |= 1;
    
    return;
}

void disableLCDDisplay(void)
{
    VIDCON0 &= ~3;
	WINCON0 &= ~1;
    
    return;
}

void changeDisplayColor(void)
{
    volatile unsigned int *ptr = (volatile unsigned int *) FRAME_BUFFER;
    unsigned int addr = 0;
    
    unsigned int colors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0, 0xFFFFFF };
    static int index = 0;
    
    // display color
    for (int y = 0; y <= LINEVAL; ++y) {
        for (int x = 0; x <= HOZVAL; ++x) {
            ptr[addr++] = colors[index];
        }
    }
    
    if (++index == 5) index = 0;
    
    return;
}