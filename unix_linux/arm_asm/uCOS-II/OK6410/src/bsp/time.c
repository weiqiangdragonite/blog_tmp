/*******************************************************************************
* File: time.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-15
*******************************************************************************/


/*

The 6410 RISC microprocessor comprises of five 32-bit timers. These timers are
used to generate internal interrupts to the ARM subsystem. In addition,
Timers 0 and 1 include a PWM function (Pulse Width Modulation), which can drive
an external I/O signal. The PWM for timer 0 and 1 have an optional dead-zone
generator capability, which can be utilized to support a large current device.
Timer 2, 3 and 4 are internal timers with no output pins.


GPF13 - PWM ECLK
GPF14 - PWM TOUT[0], CLKOUT[0]
GPF15 - PWM TOUT[1]


TCNTB - timer counter register
TCMPB - timer compare register
TCNTO - timer observation register

反转位用于控制输出电平的初始化状态。
反转位关，初始输出高电平
反转位开，初始输出低电平



PWM

PWM feature can implement by using the TCMPBn.
PWM frequency is determined by TCNTBn.
A PWM value is determined by TCMPBn.

For higher PWM value, decrease TCMPBn value. For lower PWM value, increase
TCMPBn value. If output inverter is enabled, the increment/decrement may
be opposite.

Because of double buffering feature, TCMPBn, for a next PWM cycle,
can be written in any point of current PWM cycle by ISR.

当TCNTBn减到到0时，才载入新的计数值。

占空比：
在一串理想的脉冲周期序列中（如方波），正脉冲的持续时间与脉冲总周期的比值。

例如：脉冲宽度1μs，信号周期4μs的脉冲序列占空比为0.25。

在成语中有句话：「三天打鱼，两天晒网」，如果以五天为一个周期，“打渔”的占空比则为五分之三。


TCNTBn是周期
TCMPBn用于PWM波形输出占空比的设置
例如反转位开，TCMPBn=110, TCNTBn=160(50+110), 则占空比=110/160

*/

/*

周期是频率的倒数，频率越高，周期越短

1 s = 1000 ms(毫秒) = 1000 000 us(微秒) = 1000 000 000 ns(纳秒)

频率      周期
5 MHz -> 200 ns

1 / 5 MHz = 0.2 us -> 200 ns，即每200纳秒产生一个时钟脉冲，
也就是每秒产生 1 000 000 000 / 2 = 500 000 000 个时钟脉冲，即500 MHz


以Pentium 4 2.0为例，它的工作主频为2.0GHz，这说明，每秒钟它会产生20亿个时钟脉冲信号，每个时钟信号周期为0.5ns。
频率 = 2.0 GHz = 2000 MHz = 2000 000 000 Hz(20亿个脉冲信号)
周期 = 1 / 2000 MHz = 0.0005 us = 0.5 ns


*/



#include "../app/includes.h"


/*******************************************************************************
* PWM TIMER
*******************************************************************************/

void init_pwm(void)
{
    // 使用PWM Timer0作为时间片的定时器
    // 1. 配置TCFG0和TCFG1
	// Timer input clock Frequency
    // = PCLK / ( {prescaler value + 1} ) / {divider value}
	// = 66 / (65 + 1) / 1 = 1 MHz = 1 000 000 Hz
	// 每秒产生1 000 000 个脉冲信号
	// 定时期时钟频率为每秒钟定时器减少值（每个时钟脉冲减1）
    // 时钟周期为 1 / 1 MHz = 1 us，每一时钟周期减去1
    // 定时期每1 s产生一次中断, 1 s / 1 us = 1 000 000
    // 定时器每10 ms产生一次中断，所以10 ms / 1 us = 10000
    // 定时器每1 ms产生一次中断，所以1 ms / 1 us = 1000
	TCFG0 = (TCFG0 & (~0xFF)) | 65;
	TCFG1 = 0;
    
    
    // 2. 设置计数器值(32 bits)(? ms)
    // OS_TICKS_PER_SEC = 100
    // uCOS每10ms产生一次中断
	TCNTB0 = (1000 / OS_TICKS_PER_SEC) * 1000;
    
    // 3. 打开中断源
	TINT_CSTAT |= (1 << 0);
    
    // 4. 设置定时器
	TCON = 0xB;
	// 启动定时器后要关闭Manual Update
	// 在第一次使用定时器时，需要设置“手动更新”位为1以使TCNTBn/TCMPBn寄存器的值装入
	// 内部寄存器TCNTn、TCMPn中，如果下一次还要使用该位，需要先将它清0。
	TCON &= ~2;
    
    // 5. 打开使能中断控制器
    // Timer0 Interrupt属于第0组23号
	VIC0INTENABLE |= (1 << 23);
    
    // 默认中断类型为IRQ
	
	// 6. 设置中断服务程序ISR函数地址
	VIC0VECTADDR[23] = (unsigned int) pwm_isr;
}

void pwm_isr(void)
{
    // 清除中断
    TINT_CSTAT |= (1 << 5);
	VIC0ADDRESS = 0;
    
    //
    OSTimeTick();
    
    return;
}


// not configure yet
#if OS_CPU_INT_DIS_MEAS_EN > 0
INT16U  OS_CPU_IntDisMeasTmrRd (void)
{
    INT16U  cnts;


    cnts = 0
    return (cnts);
}
#endif


// 
float pwm_frequency = 800000.0;
float percentage = 10.0;

void init_timer_1(void)
{
    // GPF15 for PWM TOUT[1] - 10
    GPFCON &= ~(3 << 30);
    GPFCON |= (2 << 30);
    
    // TCFG0 & TCFG1 same as timer 0
    // 1 MHz
    
    // first value
    TCNTB1 = (int) pwm_frequency;
    TCMPB1 = (int) (pwm_frequency * (1.0 - percentage / 100));
    percentage += 1.0;
    
    // manual update;
    TCON |= (1 << 9);
    // inverter off
    TCON &= ~(1 << 10);
    // inverter on
    //TCON |= (1 << 10);
    
    // senond value
    TCNTB1 = (int) pwm_frequency;
    TCMPB1 = (int) (pwm_frequency * (1.0 - percentage / 100));
    percentage += 1.0;
    
    // auto reload
    TCON |= (1 << 11);
    
    // int
    TINT_CSTAT |= (1 << 1);
    
    VIC0INTENABLE |= (1 << 24);
    
    VIC0VECTADDR[24] = (unsigned int) timer_1_isr;
    
    return;
}

void start_timer_1(void)
{
    // TCON for timer 1 [11:8]
    // manual update;
    //TCON |= (1 << 9);
    // start timer 1
    TCON |= (1 << 8);
    // clean manual udate
    TCON &= ~(1 << 9);
    
    return;
}

int positive = true;
void timer_1_isr(void)
{
    TCNTB1 = (int) pwm_frequency;
    TCMPB1 = (int) (pwm_frequency * (1.0 - percentage / 100));
    
    uart_printf("percentage = ");
    uart_print_int((int) percentage);
    uart_puts("%");
    
    if (positive) percentage += 1.0;
    else percentage -= 1.0;
    
    if ((int) percentage == 90) {
        positive = false;
    } else if ((int) percentage == 10) {
        positive = true;
    }
    
    // clean
    TINT_CSTAT |= (1 << 6);
    VIC0ADDRESS = 0;
    
    return;
}

void stop_timer_1(void)
{
    // stop timer 1
    TCON &= ~(1 << 8);
    
    return;
}


/*******************************************************************************
* REAL TIME CLOCK(RTC)
*******************************************************************************/

CALENDAR get_rtc_time(void)
{
    CALENDAR calendar;
    char value;
    
    // enable rtc control
    RTCCON |= (1 << 0);
    
    // second
    value = BCDSEC;
    calendar.second = ((value >> 4) & 0x7) * 10 + (value & 0xF);
    
    // minute
    value = BCDMIN;
    calendar.minute = ((value >> 4) & 0x7) * 10 + (value & 0xF);
    
    // hour
    value = BCDHOUR;
    calendar.hour = ((value >> 4) & 0x3) * 10 + (value & 0xF);
    
    // day(1 ~ 7)
    value = BCDDAY;
    calendar.day = value & 0x7;
    
    // date
    value = BCDDATE;
    calendar.date = ((value >> 4) & 0x3) * 10 + (value & 0xF);
    
    // month
    value = BCDMON;
    calendar.month = ((value >> 4) & 0x1) * 10 + (value & 0xF);
    
    // year
    value = BCDYEAR;
    calendar.year = ((value >> 4) & 0xF) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON &= ~(1 << 0);
    
    return calendar;
}

void change_rtc_time(void)
{
    int value = 0;
    uart_puts("");
    
    // get year
    while (1) {
        uart_puts("Please input year: (00 ~ 99)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 0 && value <= 99) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_year(value);
    
    // get month
    while (1) {
        uart_puts("Please input month: (1 ~ 12)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 1 && value <= 12) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_month(value);
    
    // get date
    while (1) {
        uart_puts("Please input date: (1 ~ 31)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 1 && value <= 31) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_date(value);
    
    // get day
    while (1) {
        uart_puts("Please input day: (1 ~ 7)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 1 && value <= 7) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_day(value);
    
    // get hour
    while (1) {
        uart_puts("Please input hour: (0 ~ 23)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 0 && value <= 23) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_hour(value);
    
    // get minute
    while (1) {
        uart_puts("Please input minute: (0 ~ 59)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 0 && value <= 59) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_minute(value);
    
    // get second
    while (1) {
        uart_puts("Please input second: (0 ~ 59)");
        uart_printf(" >> ");
        value = uart_get_int();
        
        if (value >= 0 && value <= 59) break;
        uart_puts("");
    }
    uart_puts("");
    change_rtc_second(value);
    
    
    //
    uart_puts("\nTime is:");
    
    return;
}

void change_rtc_year(int value)
{
    if (!(value >= 0 && value <= 99)) {
        uart_puts("\nError: Year must between 0 to 99!");
        return;
    }
    
    int low = value % 10;
    int high = value / 10;
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD year
    BCDYEAR = (high << 4) | low;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

void change_rtc_month(int value)
{
    if (!(value >= 1 && value <= 12)) {
        uart_puts("\nError: Month must between 1 to 12!");
        return;
    }
    
    int low = value % 10;
    int high = value / 10;
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD month
    BCDMON = (high << 4) | low;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

void change_rtc_date(int value)
{
    if (!(value >= 1 && value <= 31)) {
        uart_puts("\nError: Date must between 1 to 31!");
        return;
    }
    
    int low = value % 10;
    int high = value / 10;
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD date
    BCDDATE = (high << 4) | low;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

void change_rtc_day(int value)
{
    if (!(value >= 1 && value <= 7)) {
        uart_puts("\nError: Day must between 1 to 31!");
        return;
    }
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD day
    BCDDAY = value;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

void change_rtc_hour(int value)
{
    if (!(value >= 0 && value <= 23)) {
        uart_puts("\nError: Hour must between 0 to 23!");
        return;
    }
    
    int low = value % 10;
    int high = value / 10;
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD hour
    BCDHOUR = (high << 4) | low;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

void change_rtc_minute(int value)
{
    if (!(value >= 0 && value <= 59)) {
        uart_puts("\nError: Minute must between 0 to 59!");
        return;
    }
    
    int low = value % 10;
    int high = value / 10;
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD minute
    BCDMIN = (high << 4) | low;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

void change_rtc_second(int value)
{
    if (!(value >= 0 && value <= 59)) {
        uart_puts("\nError: Second must between 0 to 59!");
        return;
    }
    
    int low = value % 10;
    int high = value / 10;
    
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD second
    BCDSEC = (high << 4) | low;
    
    // disable rtc control
    RTCCON = 0;
    
    return;
}

/*******************************************************************************
* WDT
*******************************************************************************/

void init_wdt(void)
{
    
}

void reboot(void)
{
    // WTCON:
    // time = 1 / (PCLK / (Prescaler value + 1) / Division_factor)
    //      = 1 / (66 / (65 + 1) / 16)
    //      = 16 MHz = 16 000 000 Hz = 1 / 16 = 0.0625 us
    // 1 ms / 0.0625 us = 16
    // 01000001_00_1_00_0_0_1
    WTCON = 0x4121;
    
    // 
    WTDAT = 16;
    
    return;
}
