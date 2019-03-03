
#include <stdio.h>

#define GPFCON  (*((volatile unsigned long *)0x7F0080A0))
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))

#define TCFG0    (*((volatile unsigned long *)0x7F006000))
#define TCFG1    (*((volatile unsigned long *)0x7F006004))
#define TCNTB0    (*((volatile unsigned long *)0x7F00600C))
#define TCMPB0    (*((volatile unsigned long *)0x7F006010))
#define TCON       (*((volatile unsigned long *)0x7F006008))

/* 
 * 2 : 1/2, 50% 
 * 3 : 1/3, 33.3%
 */
void pwm_set(unsigned int duty)
{
	/* GPIO配置为PWM */
	GPFCON &= ~(0x3 << 28);
	GPFCON |= (0x2 << 28);     /* PWM TOUT[0] */

	/* 设置时钟源 */
	TCFG0 &= ~(0xff);
	TCFG0  |= 1;           /*  */
	TCFG1  &= ~0xf;  /* Timer0 input clock Frequency = PCLK / ( {prescaler value + 1} ) / {divider value} 
	                    *      = 66500000 / (1+1) / 1
	                    *      = 33250000
	                    */

	/* 设置TCMPB0, TCNTB0 */
	TCNTB0 = 33250;   /* PWM的频率为1KHz */							
	if (duty == 1)
	{
		TCMPB0 = 33240;
	}
	else
	{
		TCMPB0 = 33250 / duty;	
	}

	TCON &= ~(1<<2);  /* 引脚初始值为0 */
	TCON |= (1<<3);    /* auto-reload */
}

void pwm_start(void)
{
	TCON |= (1<<1);   /* set manual update */
	TCON |= (1<<0);   /* start timer 0 */
	TCON &= ~(1<<1); /* clean manual update */
}

void pwm_stop(void)
{
	TCON &= ~(1<<0);   /* stop timer 0 */
}


void pwm_menu(void)
{
	unsigned char c;
	static unsigned int cnt = 5;
	unsigned int duty;

	
	while (1)
	{
		printf("********PWM TEST MENU********\n\r");
		printf("[+] increase the PWM duty\n\r");
		printf("[-] decrease the PWM duty\n\r");
		printf("[S] stop the PWM\n\r");
		printf("[Q] quit\n\r");

		do {
			c = getc();
			if (c == '\n' || c == '\r')
			{
				printf("\n\r");
			}
			else
			{
				putc(c);
			}
		} while (c == '\n' || c == '\r');

		switch (c) {
			case '+':
			{
				duty = 100 / cnt;
				printf("duty = %d\n\r", duty);
				pwm_set(duty);
				cnt += 5;
				if (cnt > 100)
					cnt = 100;
				pwm_start();
				break;
			}

			case '-':
			{
				duty = 100 / cnt;
				printf("duty = %d\n\r", duty);
				pwm_set(duty);
				cnt -= 5;
				if (cnt < 5)
					cnt = 5;
				pwm_start();
				break;
			}


			case 's':
			case 'S':
			{
				pwm_stop();
				break;
			}

			case 'q':
			case 'Q':
			{
				return ;
				break;
			}

		}

	}
}


