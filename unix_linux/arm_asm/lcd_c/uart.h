/*******************************************************************************
* File: uart.h
* 
* Description:
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-16
*******************************************************************************/

#ifndef UART_H
#define UART_H


/* include the registers address */
#include "registers.h"


/* function prototypes */
void initUART(void);

unsigned char uartGetc(void);

void uartPutc(unsigned char ch);
void uartPuts(char *str);


#endif // UART_H