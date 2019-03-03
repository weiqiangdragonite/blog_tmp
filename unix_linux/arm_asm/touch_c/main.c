/******************************************************************************
* Projrct: 
*
* Description:
*	
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-4
******************************************************************************/

#include "uart.h"

// define the ADC register
#define		ADCCON			(*(volatile unsigned int *) (0x7E00B000))
#define		ADCDTSC			(*(volatile unsigned int *) (0x7E00B004))
#define		ADCDLY			(*(volatile unsigned int *) (0x7E00B008))
#define		ADCDAT0			(*(volatile unsigned int *) (0x7E00B00C))
#define		ADCDAT1			(*(volatile unsigned int *) (0x7E00B010))
#define		ADCUPDN			(*(volatile unsigned int *) (0x7E00B014))
// ADC & Touch Screen Interrupt Clear register
#define		ADCCLRINT		(*(volatile unsigned int *) (0x7E00B018))
#define		ADCCLRINTPNDNUP	(*(volatile unsigned int *) (0x7E00B020))

// 使能中断控制器
#define		VIC0INTENABLE	(*(volatile unsigned int *) (0x71200010))
#define		VIC1INTENABLE	(*(volatile unsigned int *) (0x71300010))
// 中断类型控制器
#define		VIC0INTSELECT	(*(volatile unsigned int *) (0x7120000C))
#define		VIC1INTSELECT	(*(volatile unsigned int *) (0x7130000C))
// 中断处理函数(ISR)地址
#define		VIC0VECTADDR	( (volatile unsigned int *) (0x71200100))
#define		VIC1VECTADDR	( (volatile unsigned int *) (0x71300100))
// 判断和清除中断控制器地址
#define		VIC0ADDRESS		(*(volatile unsigned int *) (0x71200F00))
#define		VIC1ADDRESS		(*(volatile unsigned int *) (0x71300F00))

#define		VIC1IRQSTATUS	(*(volatile unsigned int *) (0x71300000))


/* function prototype */
void initTouch(void);
void adcISR(void);
void pendnupISR(void);


/* main function */
int main(void)
{
	// init UART
	initUART();
	
	// init touch adc
	initTouch();
	
	while (1) {
		// do nothing
	}
	
	return 0;
}

void initTouch(void)
{
	// 设置预分频(PCLK最大为50 MHz???)
	// 50 MHz / (49 + 1) = 1 MHz
	ADCCON |= (1 << 14);
	ADCCON = (ADCCON & 0xFFFFC03F) | (49 << 6);
	ADCCON &= (~0x7);

	// 设置采样延时时间
	// 40000 * (1 / 3.68 MHz) = 10 ms
	// ADCDLY为40000时，按下触摸屏后，过10ms才能采样
	ADCDLY = 40000;
	
	// 打开触摸屏中断
	// INT_ADC 为63号中断(VIC1)
	// INT_PENDNUP 为62中断(VIC1)
	
	// 先清除触摸屏中断
	ADCCLRINT = 0;
	ADCCLRINTPNDNUP = 0;
	
	// 使能中断控制器中断
	VIC1INTENABLE |= 0xC0000000;
	
	// 设置为IRQ中断，默认为IRQ
	
	// 设置中断服务地址
	VIC1VECTADDR[31] = (unsigned int) adcISR;
	VIC1VECTADDR[30] = (unsigned int) pendnupISR;
	
	// 设置触摸屏为等待中断模式，等待触摸屏被按下
	ADCDTSC = 0xD3;
	
	return;
}

void adcISR(void)
{
	//uart_send("In adcISR!\n");

	// 在自动转换x/y模式下，x/y坐标都转换完后，产生INT_ADC中断
	// 先读取x/y坐标的值，再进入等待中断模式
	
	// 获取x/y坐标
	uart_send("get x and y!\n");
	
	unsigned int x = (ADCDAT0 & 0x3FF);
	unsigned int y = (ADCDAT1 & 0x3FF);
	uart_send("x = ");
	sendUartData(x);
	uart_send("y = ");
	sendUartData(y);
	
	
	// 设置触摸屏为等待中断模式，等待触摸屏松开
	ADCDTSC = 0xD3;
	ADCDTSC |= (1 << 8);
	
	
	// 清除中断
	ADCCLRINT = 0;
	VIC1ADDRESS = 0;
	
	//uart_send("Out adcISR!\n");
	
	return;
}

void pendnupISR(void)
{
	//uart_send("In pendnupISR!\n");

	// 当触摸屏被按下时，进入自动x/y轴坐标转换模式
	// 当触摸屏被松开时，进入等待中断模式
	if (ADCDAT0 & 0x8000) {
		// ADCDAT0 [15] == 1, 触摸屏松开
		// 应该可用ADCUPDN代替
		uart_send("Stylus up state!\n\n");
		
		// 设置触摸屏为等待中断模式，等待触摸屏被按下
		ADCDTSC = 0xD3;
		
	} else {
		// ADCDAT0 [15] == 0, 触摸屏被按下
		uart_send("Stylus down state!\n");
		
		// 进入自动x/y轴坐标转换模式
		//ADCDTSC = 0x9C;
		ADCDTSC = 0x0C;
		
		// 开始AD转换
		// 经过 1 / 66 MHz * 40000 = 606 us 后开始转换x坐标，再过606 us转换y坐标
		ADCCON |= 1;
	}
	
	
	// 清除中断
	ADCCLRINTPNDNUP = 0;
	VIC1ADDRESS = 0;
	
	//uart_send("Out pendnupISR!\n");

	return;
}

void whichISR(void)
{
	// 函数指针
	void (*the_isr)(void);
	the_isr = (void *) VIC1ADDRESS;
	(*the_isr)();
	
	return;
}