/******************************************************************************
* File: uart.h
* 
* Description:
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-14
******************************************************************************/


#define		GPACON			(*(volatile unsigned int *) (0x7F008000))
#define		GPAPUD			(*(volatile unsigned int *) (0x7F008008))
#define		UBRDIV0			(*(volatile unsigned int *) (0x7F005028))
#define		UDIVSLOT0		(*(volatile unsigned int *) (0x7F00502C))
#define		ULCON0			(*(volatile unsigned int *) (0x7F005000))
#define		UCON0			(*(volatile unsigned int *) (0x7F005004))
#define		UFCON0			(*(volatile unsigned int *) (0x7F005008))
#define		UMCON0			(*(volatile unsigned int *) (0x7F00500C))
#define		UTXH0			(*(volatile unsigned int *) (0x7F005020))
#define		URXH0			(*(volatile unsigned int *) (0x7F005024))
#define		UTRSTAT0		(*(volatile unsigned int *) (0x7F005010))
#define		UFSTAT0			(*(volatile unsigned int *) (0x7F005018))

void initUART(void);
void uartPutc(unsigned char c);
void sendUartData(unsigned int data);