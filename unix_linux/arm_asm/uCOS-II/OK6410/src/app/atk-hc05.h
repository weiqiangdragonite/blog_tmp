/*******************************************************************************
* File: atk-hc05.h
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-24
*******************************************************************************/


#ifndef ATK_HC05
#define ATK_HC05

void hc05_test(void);

void init_hc05(void);
void set_hc05(void);

void hc05_enter_at(void);
void hc05_exit_at(void);
void hc05_cmd(char *str);

int hc05_state(void);
void hc05_putc(unsigned char ch);
void hc05_puts(char *str);


void init_uart_1(int baud_rate);

void uart_1_putc(unsigned char ch);
void uart_1_puts(char *str);
void uart_1_printf(char *str);

void uart_1_gets(void);
unsigned char uart_1_getc(void);

void uart_1_isr(void);


#endif  // ATK_HC05
