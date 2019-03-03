/******************************************************************************
* File: uart.c
* 
* Description:
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-14
******************************************************************************/

#include "uart.h"

void initUART(void)
{
	// 使用UART0
	
	// 0. 设置引脚GPA0和GPA1为使用串口模式
	// 因为OK6410的COM0串口的RXD0和TXD0连接GPIO的GPA0和GPA1
	// 低8位设为 0010_0010
	GPACON = (GPACON & 0xFFFFFF00) | 0x00000022;
	
	// 1. 通过UBRDIVn设置波特率为115200bps
	// DIV_VAL = (PCLK / (bps * 16)) - 1
	//         = (66000000 / (115200 * 16)) - 1
	//         = 34.8
	// 13 / 16 = 0.8125
	// 所以UBRDIVn=34，UDIVSLOTn=0xDFDD
	UBRDIV0 = 34;
	UDIVSLOT0 = 0xDFDD;
	
	// 2. 通过ULCONn设置传输格式
	// 普通工作模式Normal mode operation
	// 无校验位No parity
	// 1位停止位
	// 数据位为8位
	// 0000_0011
	ULCON0 = 3;
	
	// 3.通过UCONn设置通道工作模式
	// 时钟源为PCLK
	// 中断请求类型为脉冲中断或电平中断（脉冲中断就是就是上升沿或下降沿）
	// 禁止接收超时中断
	// 不产生接收错误状态中断
	// 发送和接收模式为中断或轮询
	// 1000_0000_0101
	UCON0 = (UCON0 & 0xFFFFF000) | 0x805;
	
	// 设置UFCONn为禁止FIFO模式
	UFCON0 = 0;
	// 设置UMCONn为禁止自动流控制
	UMCON0 = 0;
	
	return;
}

void sendUartData(unsigned int data)
{
	//unsigned char *addr = &data
	// 用十进制或十六进制都行
	uartPutc(10);
	uartPutc(33);
	//unsigned char str[] = "hello, world\n";
	//for (int i = 0; str[i] != '\n'; ++i) {
	//	uartPutc(str[i]);
	//}
	
	volatile char num[10];
	volatile int i = 0;
	while (1) {
		num[i++] = data % 10 + '0';
		data /= 10;
		if (data == 0) break;
	}
	for (i -= 1; i >= 0; --i) {
		uartPutc(num[i]);
	}
	
	
	return;
}

void uartPutc(unsigned char c)
{
	// 发送字符
	while (1) {
		if ((UTRSTAT0 & 6) == 6) break;
	}
	UTXH0 = c;
	
	return;
}