/*******************************************************************************
* File: uart.h
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-29
*******************************************************************************/

#ifndef UART_H
#define UART_H

#include "registers.h"


/* function prototype */

void init_uart(void);

void uart_putc(unsigned char ch);
void uart_puts(char *str);

unsigned char uart_getc(void);
void uart_gets(char *str);


void uart_printf(char *str);
void uart_scanf(char *type, int *addr);


int uart_get_int(void);
void uart_print_int(int data);


void int_to_hex(unsigned int data, char *num);
void uart_print_hex(unsigned int data);
void uart_print_addr(unsigned int data);


int uart_get_file(unsigned char *buffer);


#endif