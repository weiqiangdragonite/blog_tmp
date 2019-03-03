/*******************************************************************************
* File: time.h
*
* By: weiqiangdragonite@gmail.com
* Last updated: 2013-10-6
*******************************************************************************/


#ifndef TIME_H
#define TIME_H


/* function prototype */

// pwm
void init_pwm(void);
void pwm_isr(void);


// rtc
void read_rtc_time(void);
void bcd_to_ascii(char value);
int get_rtc_second(void);
int get_rtc_minute(void);
int get_rtc_hour(void);
int get_rtc_date(void);
int get_rtc_day(void);
int get_rtc_month(void);
int get_rtc_year(void);
void change_rtc_time(void);
void change_rtc_year(int value);
void change_rtc_month(int value);
void change_rtc_date(int value);
void change_rtc_day(int value);
void change_rtc_hour(int value);
void change_rtc_minute(int value);
void change_rtc_second(int value);


// wdt
void reboot(void);


#endif  // TIME_H