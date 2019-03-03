/******************************************************************************
* Projrct: 
*
* Description:
*	使用定时器读取ADC的值，并用串口输出
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-4
******************************************************************************/

#include "uart.h"

// define the ADC register
#define		ADCCON			(*(volatile unsigned int *) (0x7E00B000))
#define		ADCDAT0			(*(volatile unsigned int *) (0x7E00B00C))

// define the pwm register
#define		TCFG0			(*(volatile unsigned int *) (0x7F006000))
#define		TCFG1			(*(volatile unsigned int *) (0x7F006004))
#define		TCNTB0			(*(volatile unsigned int *) (0x7F00600C))
#define		TCMPB0			(*(volatile unsigned int *) (0x7F006010))
#define		TCNTO0			(*(volatile unsigned int *) (0x7F006014))
#define		TCON			(*(volatile unsigned int *) (0x7F006008))
#define		TINT_CSTAT		(*(volatile unsigned int *) (0x7F006044))

// 第0组使能中断地址
#define		VIC0INTENABLE	(*(volatile unsigned int *) (0x71200010))
// 第0组中断处理函数(ISR)地址
#define		VIC0VECTADDR	( (volatile unsigned int *) (0x71200100))
// 第0组清除中断控制器地址
#define		VIC0ADDRESS		(*(volatile unsigned int *) (0x71200F00))


/* function prototype */
void initADC(void);
void initPWM(void);
void pwmISR(void);

volatile unsigned int data = 0;


/* main function */
int main(void)
{
	// init ADC
	initADC();
	
	// init UART
	initUART();

	// set the PWM
	initPWM();
	
	while (1) {
		// do nothing
	}
	
	return 0;
}

void initADC(void)
{
	// 默认采用10位精度，
	// 最大分频时钟为5 MHz，
	// A/D converter freq = HCLK / (PRSCVL + 1)
	//                    = 66 MHz / (65 + 1)
	//                    = 1 MHz (最大为5 MHz)
	// Conversion time = 1 / (1 MHz / (5cycles)) = 5 us
	ADCCON |= (1 << 14);
	ADCCON = (ADCCON & 0xFFFFC03F) | (65 << 6);
	ADCCON &= 0xFFFFFFFB;
	
	// 开始AD转换
	ADCCON |= 1;
	
	// 等待开始
	while (1) {
		if (!(ADCCON & 1)) break;
	}

	return;
}

void initPWM(void)
{
	// 1. 配置TCFG0和TCFG1
	// Timer input clock Frequency = PCLK / ( {prescaler value + 1} ) / {divider value}
	// = 66 / (65 + 1) / 1 = 1 MHz = 1000 000 Hz
	TCFG0 = (TCFG0 & 0xFFFFFF00) | 65;
	TCFG1 = 0;
	
	// 2. 设置计数器值
	TCNTB0 = 2000000;
	
	// 3. 打开中断
	TINT_CSTAT = 1;
	
	// 4. 设置定时器
	TCON = 0xB;
	// 启动定时器后要关闭Manual Update
	TCON &= 0xFFFFFFFD;
	
	// 5. 打开使能中断控制器
	VIC0INTENABLE |= (1 << 23);
	
	// 6. 设置中断服务程序ISR函数地址
	VIC0VECTADDR[23] = (unsigned int) pwmISR;
	
	return;
}

void pwmISR(void)
{

	// 处理中断
	// 检查AD转换是否结束
	while (1) {
		if (ADCCON & 0x8000) break;
	}
	data = (unsigned int) (ADCDAT0 & 0x3FF);
	sendUartData(data);
	
	
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
