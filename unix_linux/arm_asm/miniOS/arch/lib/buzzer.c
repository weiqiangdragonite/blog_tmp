

#include "buzzer.h"

void init_buzzer(void)
{
    // 设置GPFCON，引脚设为输出模式
    GPFCON &= ~(3 << 30);
    GPFCON |= (1 << 30);
    
    // 设置GPFPUD，上下拉电阻禁止
    GPFPUD &= ~(3 << 30);

    return;
}

void turn_on_buzzer(void)
{
    // 蜂鸣器响, 共16位，最高位为1
    GPFDAT |= (1 << 15);
    
    return;
}

void turn_off_buzzer(void)
{
    GPFDAT &= ~(1 << 15);
    
    return;
}