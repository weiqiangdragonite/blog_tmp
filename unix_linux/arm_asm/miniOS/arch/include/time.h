


#ifndef TIME_H
#define TIME_H

#include "registers.h"


void init_pwm(void);
void pwm_isr(void);
extern void round_robin(void);



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


#endif  // TIME_H
