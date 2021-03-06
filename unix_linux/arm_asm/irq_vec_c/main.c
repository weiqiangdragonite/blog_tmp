/******************************************************************************
* Projrct: 
*
* Description:
*	使用定时器中断点亮LED。
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-4
******************************************************************************/


#define		GPMDAT		(*(volatile unsigned int *) (0x7F008824))
// define the pwm register
#define		TCFG0		(*(volatile unsigned int *) (0x7F006000))
#define		TCFG1		(*(volatile unsigned int *) (0x7F006004))
#define		TCNTB0		(*(volatile unsigned int *) (0x7F00600C))
#define		TCMPB0		(*(volatile unsigned int *) (0x7F006010))
#define		TCNTO0		(*(volatile unsigned int *) (0x7F006014))
#define		TCON		(*(volatile unsigned int *) (0x7F006008))
#define		TINT_CSTAT	(*(volatile unsigned int *) (0x7F006044))

// 第0组使能中断地址
#define		VIC0INTENABLE	(*(volatile unsigned int *) (0x71200010))
// 第0组中断处理函数(ISR)地址
#define		VIC0VECTADDR	( (volatile unsigned int *) (0x71200100))
// 第0组清除中断控制器地址
#define		VIC0ADDRESS		(*(volatile unsigned int *) (0x71200F00))


/* function prototype */
void initLED(void);
void initPWM(void);
void timerISR(void);


/* main function */
int main(void)
{

	// init the LED
	initLED();
	
	// set the PWM
	initPWM();
	
	while (1) {
		// do nothing
	}
	
	return 0;
}

void initPWM(void)
{
	// 1. 配置TCFG0和TCFG1
	// Timer input clock Frequency = PCLK / ( {prescaler value + 1} ) / {divider value}
	// = 66 / (65 + 1) / 1 = 1 MHz = 1000 000 Hz
	TCFG0 = (TCFG0 & 0xFFFFFF00) | 65;
	TCFG1 = 0;
	
	// 2. 设置计数器值
	TCNTB0 = 1000000;
	
	// 3. 打开中断
	TINT_CSTAT = 1;
	
	// 4. 设置定时器
	TCON = 0xB;
	// 启动定时器后要关闭Manual Update
	TCON &= 0xFFFFFFFD;
	
	// 5. 打开使能中断控制器
	VIC0INTENABLE |= (1 << 23);
	
	// 6. 设置中断服务程序ISR函数地址
	VIC0VECTADDR[23] = (unsigned int) timerISR;
	
}

void timerISR(void)
{

	// 处理中断
	if (GPMDAT & 0xF) {
		// turn on
		GPMDAT &= 0xFFFFFFF0;
	} else {
		// turn off
		GPMDAT |= 0x0000000F;
	}
	
	// 3. 清除中断
	TINT_CSTAT |= (1 << 5);
	VIC0ADDRESS = 0;
	
	return;
}

void whichISR(void)
{
	// 函数指针
	void (*the_isr)(void);
	the_isr = (void *) VIC0ADDRESS;
	(*the_isr)();
	
	return;
}