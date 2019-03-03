/*******************************************************************************
* File: time.c
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-6
*******************************************************************************/


#include "../app/includes.h"


/*******************************************************************************
* PWM
*******************************************************************************/

void init_pwm(void)
{
    // 使用PWM Timer0作为时间片的定时器
    // 1. 配置TCFG0和TCFG1
	// Timer input clock Frequency
    // = PCLK / ( {prescaler value + 1} ) / {divider value}
	// = 66 / (65 + 1) / 1 = 1 MHz = 1 000 000 Hz
    // = 66 / ()
    // 时钟周期为 1 / 1 MHz = 1 us，每一时钟周期减去1
    // 定时器每10 ms产生一次中断，所以10 ms / 1 us = 10000
    // 定时器每1 ms产生一次中断，所以1 ms / 1 us = 1000
	TCFG0 = (TCFG0 & (~0xFF)) | 65;
	TCFG1 = 0;
    
    
    // 2. 设置计数器值(32 bits)(? ms)
	TCNTB0 = (1000 / OS_TICKS_PER_SEC) * 1000;
    
    // 3. 打开中断源
	TINT_CSTAT = 1;
    
    // 4. 设置定时器
	TCON = 0xB;
	// 启动定时器后要关闭Manual Update
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


/*******************************************************************************
* RTC
*******************************************************************************/

void read_rtc_time(void)
{
    int value = 0;
    
    // bcd year
    value = get_rtc_year();
    uart_print_int(value);
    uart_printf("-");
    
    // bcd month
    value = get_rtc_month();
    uart_print_int(value);
    uart_printf("-");
    
    // bcd date
    value = get_rtc_date();
    uart_print_int(value);
    uart_printf(" (");
    
    // bcd day
    value = get_rtc_day();
    uart_print_int(value);
    uart_printf(")  ");
    
    // bcd hour
    value = get_rtc_hour();
    uart_print_int(value);
    uart_printf(":");
    
    // bcd minute
    value = get_rtc_minute();
    uart_print_int(value);
    uart_printf(":");
    
    // bcd second
    value = get_rtc_second();
    uart_print_int(value);

    return;
}

int get_rtc_year(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD year
    char value = BCDYEAR;
    // convert to int
    int year = ((value >> 4) & 0xF) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON = 0;
    
    return year;
}

int get_rtc_month(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD month
    char value = BCDMON;
    // convert to int
    int month = ((value >> 4) & 0x1) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON = 0;
    
    return month;
}

int get_rtc_day(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD day
    char value = BCDDAY;
    // convert to int
    int day = value & 0x7;
    
    // disable rtc control
    RTCCON = 0;
    
    return day;
}

int get_rtc_date(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD date
    char value = BCDDATE;
    // convert to int
    int date = ((value >> 4) & 0x3) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON = 0;
    
    return date;
}

int get_rtc_hour(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD hour
    char value = BCDHOUR;
    // convert to int
    int hour = ((value >> 4) & 0x3) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON = 0;
    
    return hour;
}

int get_rtc_minute(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD minute
    char value = BCDMIN;
    // convert to int
    int minute = ((value >> 4) & 0x7) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON = 0;
    
    return minute;
}

int get_rtc_second(void)
{
    // reset rtc counter
    RTCCON |= (1 << 3);
    
    // enable rtc control
    RTCCON = 1;
    
    // BCD second
    char value = BCDSEC;
    // convert to int
    int second = ((value >> 4) & 0x7) * 10 + (value & 0xF);
    
    // disable rtc control
    RTCCON = 0;
    
    return second;
}

void bcd_to_ascii(char value)
{
    // low 4 bits [3:0]
    char low = (value & 0xF) + 0x30;
    
    // high 4 bits [7:4]
    char high = (value & 0xF0) + 0x30;
    
    //
    uart_putc(high);
    uart_putc(low);
    
    return;
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
    read_rtc_time();
    
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