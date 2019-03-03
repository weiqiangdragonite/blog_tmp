/*******************************************************************************
* File: uart.h
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-15
*******************************************************************************/


#ifndef BSP_UART_H
#define BSP_UART_H


#define UART_0  0
#define UART_1  1
#define UART_2  2
#define UART_3  3



/* function prototype */

void init_uart(void);

void set_uart_baud_rate(int uart, int baud_rate);
void uart_0_isr(void);

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
int uart_get_file_size(void);





#endif  // BSP_UART_H
