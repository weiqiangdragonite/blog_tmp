/*******************************************************************************
* File: uart.c
*
* By weiqiangdragonite@gmail.com
* Update Date: 2013-8-30
*******************************************************************************/

/*

\a  响铃(BEL)                                 7
\b  退格(BS) ，将当前位置移到前一列           8
\f  换页(FF)，将当前位置移到下页开头          12
\n  换行(LF) ，将当前位置移到下一行开头       10
\r  回车(CR) ，将当前位置移到本行开头         13
\t  水平制表(HT) （跳到下一个TAB位置）        9
\v  垂直制表(VT)                              11
\\  代表一个反斜线字符''\'
\'  代表一个单引号（撇号）字符
\'' 代表一个双引号字符
\0  空字符(NULL)                               0

*/

#ifndef NULL
#   define NULL 0
#endif

#include "uart.h"

void init_uart(void)
{
    // 0. set GPA0 and GPA1 to UART
    // the OK6410 RXD0 is connect to GPA0
    // TXD0 is connect to GPA1
	// set GPACON[7:0] to 0010_0010
	GPACON = (GPACON & ~0xFF) | 0x22;
	
	// 1. set the baud rate to 115200bps
	// DIV_VAL = (PCLK / (bps * 16)) - 1
	//         = (66000000 / (115200 * 16)) - 1
	//         = 34.8
	// 13 / 16 = 0.8125
	// UBRDIVn=34，UDIVSLOTn=0xDFDD
	UBRDIV0 = 34;
	UDIVSLOT0 = 0xDFDD;
	
	// 2. set the ULCONn
	// Normal mode operation
	// No parity
	// One stop bit per frame
	// 8-bit data
	// 0000_0011
	ULCON0 = 3;
	
	// 3. set the UCONn
	// clock is: PCLK
	// Interrupt type is Level
	// Disable time-out interrupt
	// Do not generate error status interrupt
	// Transmit/Receive mode is interrupt or polling
	// 1011_0000_0101
	UCON0 = (UCON0 & ~0xFFF) | 0xB05;
	
	// set the UFCONn to disable FIFO
	UFCON0 = 0;
	// set the UMCONn to disable flow control
	UMCON0 = 0;
    
    uart_puts("");
    
	return;
}

void uart_putc(unsigned char ch)
{
    while (1) {
		if ((UTRSTAT0 & 6) == 6) break;
	}
	UTXH0 = ch;
    if (ch == '\n') uart_putc('\r');
	
	return;
}

void uart_puts(char *str)
{
    
    for (int i = 0; str[i] != '\0'; ++i) {
		uart_putc(str[i]);
	}
    
    uart_putc('\n');
    
    return;
}

void uart_printf(char *str)
{
    for (int i = 0; str[i] != '\0'; ++i) {
		uart_putc(str[i]);
	}
    
    return;
}




void int_to_hex(unsigned int data, char *num)
{
    //char num[10];
    int i = 0;
    int remainder = 0;
    
    while (1) {
        remainder = data % 16;
        data /= 16;
        
        if (remainder >= 10 && remainder <= 15) {
            num[i++] = remainder - 10 + 'A';
        } else {
            num[i++] = remainder + '0';
        }
        
        if (data == 0 && i == 8) break;
    }
    num[i] = '\0';
    
    return;
}

void uart_print_hex(unsigned int data)
{
    char num[10];
    
    int_to_hex(data, num);
    
    for (int i = 7; i >= 0; --i) {
        uart_putc(num[i]);
    }
    
    return;
}

void uart_print_addr(unsigned int data)
{
    char num[10];
    
    int_to_hex(data, num);
    
    // EA_00_00_06 -> 06_00_00_EA
    uart_putc(num[1]);
    uart_putc(num[0]);
    uart_putc(num[3]);
    uart_putc(num[2]);
    uart_putc(num[5]);
    uart_putc(num[4]);
    uart_putc(num[7]);
    uart_putc(num[6]);
    
    return;
}




unsigned char uart_getc(void)
{
    char buffer[80];
    int i = 0;
    
    while (1) {
        // get the char
        while (1) {
            if (UTRSTAT0 & 1) break;
        }
        buffer[i] = URXH0;
        
        // be careful, do not use  up/down/left/right key
        // backspace
        if (buffer[i] == 8 && i > 0) {
            uart_putc(buffer[i--]);
            buffer[i] = ' ';
            uart_putc(buffer[i]);
            uart_putc(8);
            continue;
        } else if (buffer[i] == 8 && i == 0) {
            continue;
        }
        
        // send to screen what you type
        if (i != 79) {
            uart_putc(buffer[i]);
        } else if (i == 79 && buffer[i] == 0xD) {
            uart_puts("");
            return buffer[0];
        }
        
        
        // 
        if (i == 0 && buffer[i] == 0xD) {
            uart_puts("");
            return '\0';
        }
        
        //
        if (i > 0 && buffer[i] == 0xD) {
            uart_puts("");
            return buffer[0];
        }
        
        // 
        if (++i == 80) i = 79;
    }
    
}

void uart_gets(char *buffer)
{
    
    int i = 0;
    
    while (1) {
        // get the char
        while (1) {
            if (UTRSTAT0 & 1) break;
        }
        buffer[i] = URXH0;
        
        // be careful, do not use  up/down/left/right key
        // backspace
        if (buffer[i] == 8 && i > 0) {
            uart_putc(buffer[i--]);
            buffer[i] = ' ';
            uart_putc(buffer[i]);
            uart_putc(8);
            continue;
        } else if (buffer[i] == 8 && i == 0) {
            continue;
        }
        
        // send to screen what you type
        if (i != 79) {
            uart_putc(buffer[i]);
        } else if (i == 79 && buffer[i] == 0xD) {
            uart_puts("");
            buffer[i] = '\0';
            return;
        }
        
        
        // 
        if (i == 0 && buffer[i] == 0xD) {
            buffer[i] = '\0';
            uart_puts("");
            return;
        }
        
        //
        if (i > 0 && buffer[i] == 0xD) {
            uart_puts("");
            buffer[i] = '\0';
            return;
        }
        
        // 
        if (++i == 80) i = 79;
    }
    
}


void uart_scanf(char *type, int *addr)
{
    
}


int uart_get_int(void)
{
    char buffer[80];
    uart_gets(buffer);
    int num = 0;
    
    int i = 0;
    int sign = 1;
    if (buffer[0] == '-') {
        ++i;
        sign = -1;
    }

    for (; buffer[i] != '\0'; ++i) {
        if (!(buffer[i] >= '0' && buffer[i] <= '9')) return 0;
        num = num * 10 + buffer[i] - '0';
    }

    return sign * num;
}

void uart_print_int(int data)
{
    char num[80];
    int i = 0;
    
    while (1) {
		num[i++] = data % 10 + '0';
		data /= 10;
		if (data == 0) break;
	}
    
    if (data < 0) uart_putc('-');
    
    for (i -= 1; i >= 0; --i) {
		uart_putc(num[i]);
	}
    
    return;
}



int uart_get_file(unsigned char *buffer)
{

    if (UTRSTAT0 & 1) {
        *buffer = URXH0;
        return 1;
    }
    
    return 0;
}

int uart_get_file_size(void)
{
    char buffer[80];
    int i = 0;
    while (1) {
        // get the char
        while (1) {
            if (UTRSTAT0 & 1) break;
        }
        buffer[i] = URXH0;
        
        if (buffer[i] == '\r') {
            buffer[i] = '\0';
            break;
        }
        
        ++i;
    }

    int num = 0;
    i = 0;
    for (; buffer[i] != '\0'; ++i) {
        if (!(buffer[i] >= '0' && buffer[i] <= '9')) return 0;
        num = num * 10 + buffer[i] - '0';
    }

    return num;
}
