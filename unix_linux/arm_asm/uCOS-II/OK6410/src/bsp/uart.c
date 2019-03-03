/*******************************************************************************
* File: uart.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-24
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


换行
\r      Mac 
\n      Unix/Linux 
\r\n    Windows

*/

/*

s3c6410有4个uart

RXD0 -> GPA0
TXD0 -> GPA1

RXD1 -> GPA4
TXD1 -> GPA5

RXD2 -> GPB0
TXD2 -> GPB1

RXD3 -> GPB2
TXD3 -> GPB3


*/


#include "../app/includes.h"


void init_uart(void)
{
    // 0. use UART0
    // set GPA0 and GPA1 to UART
    // the OK6410 RXD0 is connect to GPA0
    // TXD0 is connect to GPA1
	// set GPACON[7:0] to 0010_0010
	GPACON = (GPACON & ~0xFF) | 0x22;
	
	// RXD1 & TXD1 - GPA4 & GPA5 : [23:16]
	//GPACON = (GPACON & ~0xFF0000) | 0x22;
	// RXD2 & TXD2 - GPB0 & GPB1 : [7:0]
	//GPBCON = (GPBCON & ~0xFF) | 0x22;
	// RXD3 & TXD3 - GPB2 & GPB3 : [15:8]
	//GPBCON = (GPBCON & ~0xFF00) | 0x22
	
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


    // mask interrupt
	UINTM0 = 0xF;
	UINTM1 = 0xF;
	UINTM2 = 0xF;
	UINTM3 = 0xF;
	
	
	//VIC1INTENABLE |= (1 << 5);
	//VIC1VECTADDR[5] = (unsigned int) uart_0_isr;
    
    uart_puts("");
    
	return;
}

void set_uart_baud_rate(int uart, int baud_rate)
{
    //
    int UBRDIVn;
    int UDIVSLOTn;
    
    // set user define baud rate
	float DIV_VAL = (float) 66000000 / (baud_rate * 16) - 1;
	UBRDIVn = (int) DIV_VAL;
	float point = DIV_VAL - UBRDIVn;
	
	int i = 0;
	while (true) {
	    if ((i / 16.0) >= point) break;
	    i++;
	}
	
	switch (i) {
	    case  0: UDIVSLOTn = 0x0000; break;
	    case  1: UDIVSLOTn = 0x0080; break;
	    case  2: UDIVSLOTn = 0x0808; break;
	    case  3: UDIVSLOTn = 0x0888; break;
	    case  4: UDIVSLOTn = 0x2222; break;
	    case  5: UDIVSLOTn = 0x4924; break;
	    case  6: UDIVSLOTn = 0x4A52; break;
	    case  7: UDIVSLOTn = 0x54AA; break;
	    case  8: UDIVSLOTn = 0x5555; break;
	    case  9: UDIVSLOTn = 0xD555; break;
	    case 10: UDIVSLOTn = 0xD5D5; break;
	    case 11: UDIVSLOTn = 0xDDD5; break;
	    case 12: UDIVSLOTn = 0xDDDD; break;
	    case 13: UDIVSLOTn = 0xDFDD; break;
	    case 14: UDIVSLOTn = 0xDFDF; break;
	    case 15: UDIVSLOTn = 0xFFDF; break;
	    case 16: UBRDIVn += 1; UDIVSLOTn = 0; break;
	}
	
	if (uart == UART_0) {
	    UBRDIV0 = UBRDIVn;
	    UDIVSLOT0 = UDIVSLOTn;
	} else if (uart == UART_1) {
	    UBRDIV1 = UBRDIVn;
	    UDIVSLOT1 = UDIVSLOTn;
	} else if (uart == UART_2) {
	    UBRDIV2 = UBRDIVn;
	    UDIVSLOT2 = UDIVSLOTn;
	} else if (uart == UART_3) {
	    UBRDIV3 = UBRDIVn;
	    UDIVSLOT3 = UDIVSLOTn;
	}
	
    //uart_printf("baud rate = ");
    //uart_print_int(baud_rate);
    //uart_printf("; UBRDIVn = ");
    //uart_print_int(UBRDIVn);
    //uart_printf("; UDIVSLOTn = ");
    //uart_print_hex(UDIVSLOTn);
    //uart_puts("");
    	
	return;
}


void uart_0_isr(void)
{
    uart_puts("uart 0 isr");
    
    //clear interrupt
    UINTP0 = 0xF;
    VIC1ADDRESS = 0;
    
    return;
}


void uart_putc(unsigned char ch)
{
    // new line is \r\n -> 0x0D 0x0A
    //if (ch == '\n') uart_putc('\r');
    
    while (1) {
		if ((UTRSTAT0 & 6) == 6) break;
	}
	UTXH0 = ch;
	
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
