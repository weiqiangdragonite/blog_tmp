

#include "key.h"
#include "buzzer.h"


void init_key(void)
{
    // 6个用户按键对应GPN0 ~ GPN5，设为外部中断模式，即10
    GPNCON &= ~0xFFF;
    GPNCON |= 0xAAA;
    
    // GPNPUD设为上下拉禁止
    GPNPUD &= ~0xFFF;
    
    // 设置中断信号类型为上升沿有效（即按键松开产生中断）
    // 按键源中断为EINT0 ~ EINT5
    EINT0CON0 &= ~0xFFF;
    EINT0CON0 |= 0x444;
    
    // 在中断源中使能中断，设置屏蔽中断寄存器，取消屏蔽，低6位设为0
    EINT0MASK &= ~0x3F;
    
    // 在中断控制器中使能这些中断
    // EINT0 ~ EINT3 属于第0组，EINT4 ~ EINT5 属于第1组
    VIC0INTENABLE |= 3;
    
    // 设置中断类型，默认为IRQ
    
    // 设置中断服务程序ISR函数地址
    VIC0VECTADDR[0] = (unsigned int) key_isr;
    VIC0VECTADDR[1] = (unsigned int) key_isr;
    
    return;
}

void key_isr(void)
{
    // 判断中断源，使用EINT0PEND
    // 如果低6位中的某一位为1，则说明有中断产生，即有按键按下或松开
    // 哪一位为1，就是哪一个按键有中断
    // 再使用GPNDAT取出该位的值，如果为1，是高电平，说明按键松开
    // 如果为0，是低电平，按键按下
    unsigned int val = EINT0PEND & 0x3F;
    
    // 处理中断
    switch (val) {
        case 1:
            // 按键1产生中断
            key_1_handler();
            break;
        case 2:
            // 按键2产生中断
            key_2_handler();
            break;
        case 4:
            // 按键3产生中断
            key_3_handler();
            break;
        case 8:
            // 按键4产生中断
            key_4_handler();
            break;
        case 0x10:
            // 按键5产生中断
            key_5_handler();
            break;
        case 0x30:
            // 按键6产生中断
            key_6_handler();
            break;
        default:
            break;
    }
    
    // 清除中断
    EINT0PEND |= 0x3F;
    VIC0ADDRESS = 0;
    
    return;
}

void key_1_handler(void)
{
    if (GPMDAT & 1) {
        // turn on led 1
        GPMDAT &= ~1;
    } else {
        // turn off led 1
        GPMDAT |= 1;
    }
    
    return;
}

void key_2_handler(void)
{
    if (GPMDAT & 2) {
        // turn on led 2
        GPMDAT &= ~2;
    } else {
        // turn off led 2
        GPMDAT |= 2;
    }
    
    return;
}

void key_3_handler(void)
{
    if (GPMDAT & 4) {
        // turn on led 3
        GPMDAT &= ~4;
    } else {
        // turn off led 3
        GPMDAT |= 4;
    }
    
    return;
}

void key_4_handler(void)
{
    if (GPMDAT & 8) {
        // turn on led 4
        GPMDAT &= ~8;
    } else {
        // turn off led 4
        GPMDAT |= 8;
    }
    
    return;
}

void key_5_handler(void)
{
    // 判断按键5是按下还是松开
    //unsigned int val = GPNDAT;
    
    if ((GPFDAT >> 15) & 1) {
        turn_off_buzzer();
    } else {
        turn_on_buzzer();
    }
    
    return;
}

void key_6_handler(void)
{
    return;
}