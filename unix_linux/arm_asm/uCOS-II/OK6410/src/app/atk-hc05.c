/*******************************************************************************
* File: atk-hc05.c
*
* Description:
*   For uC/OS-II(OK6410)
*
* By: weiqiangdragonite@gmail.com
* Updated: 2013-10-24
*******************************************************************************/


#include "includes.h"


/*

 OK6410:     ATK-HC05:
    GPP1 <-- LED (connected, output high level, otherwise, low level)
    GPP8 --> KEY (high level is effective)
    TXD1 --> RXD
    RXD1 <-- TXD
    VCC  --- VCC
    GND  --- GND
    
KEY高电平时处于AT状态，低电平时处于收发状态

*/


#define buffer_size 1024
char uart1_buffer[buffer_size];
int buffer_index = 0;
int receive_over = false;


//
int AT_MODE = false;

void init_hc05(void)
{
    // GPP1[3:2] set to Input mode - 00
    GPPCON &= ~0xC;
    
    // GPP8[17:16] set to Output mode - 01
    GPPCON &= ~0x30000;
    GPPCON |= 0x10000;
    
    // GPPPUD1[3:2] set to pull-down enabled - 01
    GPPPUD &= ~0xC;
    GPPPUD |= 0x4;
    
    // GPPPUD8[17:16] set to pull-down enabled - 01
    GPPPUD &= ~0x30000;
    GPPPUD |= 0x10000;
    
    // default(same as uart in AT mode)
    init_uart_1(38400);
    
    // mode != 1 (we start in this mode)
    // if KEY set to empty(connect to GND), module will enter PAIR mode,
    // the SAT led will flash twice every second,
    // but after this, if set KEY to high level, module will also enter
    // AT mode, but led still flash twice per second
    hc05_exit_at();
    AT_MODE = false;
    
    // mode == 1
    // if KEY set to high level(connect to VCC), module will enter AT mode,
    // the SAT led will flash one time every second;

    return;
}


void set_hc05(void)
{
    // enable IRQ for UART 1 interrupt
    __asm__ ("msr cpsr_c, #0x1F\n");
    
    // init ATK-HC05
    init_hc05();
    
    // set KEY to high level
    hc05_enter_at();
    
    int delay = 0xFFFF;
    while (--delay);

    // enter AT mode
    hc05_cmd("AT+RESET\r\n");
    
    hc05_cmd("AT\r\n");
    delay = 0xFFFF;
    while (--delay);
    
    // 
    hc05_cmd("AT+RESET\r\n");
    delay = 0xFFFFF;
    while (--delay);
    
    AT_MODE = true;
    
    //
    char buffer[80];
    while (true) {
        uart_puts("\nPlease input command (empty to return Bootloader Menu)");
        uart_printf(" >> ");
        uart_gets(buffer);
        uart_puts("");
        
        //
        if (buffer[0] == '\0') break;
        
        int i = 0;
        while (true) {
            if (buffer[i] == '\0') break;
            ++i;
        }
        buffer[i] = '\r';
        buffer[i+1] = '\n';
        buffer[i+2] = '\0';
        
        uart_printf("HC05 : ");
        hc05_cmd(buffer);
        
        // wait
        while (true) {
            if (UTRSTAT1 & 1) break;
        }
        delay = 0xFFFF;
        while (--delay);
    }
    
    hc05_exit_at();
    AT_MODE = false;
    // restart module
    //uart_printf("HC05: ");
    hc05_cmd("AT+RESET\r\n");
    //while (true) {
    //    if (UTRSTAT1 & 1) break;
    //}
    delay = 0xFFFF;
    while (--delay);
    
    return;
}

void hc05_test(void)
{

}

void hc05_enter_at(void)
{
    // set GPP8 to high level
    GPPDAT |= 0x100;
    
    return;
}

void hc05_exit_at(void)
{
    // set GPP8 to low level
    GPPDAT &= ~0x100;
    
    return;
}


void hc05_cmd(char *str)
{
    uart_1_puts(str);
    
    return;
}

int hc05_state(void)
{
    // if paired, GPP1 is set to high level
    return (GPPDAT &= 0x2);
}


void hc05_putc(unsigned char ch)
{
    uart_1_putc(ch);
    
    return;
}

void hc05_puts(char *str)
{
    uart_1_puts(str);
    
    return;
}



////////////////////////////////////////////////////////////////////////////////

void init_uart_1(int baud_rate)
{
    // 0. use UART1
	// TXD1 connect to GPA5
    // RXD1 connect to GPA4
    // GPA5[23:20] set to UART TXD[1] mode - 0010
    // GPA4[19:16] set to UART RXD[1] mode - 0010
    GPACON &= ~0xFF0000;
    GPACON |=  0x220000;
    
    // GPAPUD[11:8] set to pull-up/down disabled - 0000
    GPAPUD &= ~0xF00;
	
	// 1. set the baud rate
	set_uart_baud_rate(UART_1, baud_rate);
	
	// 2. set the ULCONn
	// Normal mode operation
	// No parity
	// One stop bit per frame
	// 8-bit data
	// 0000_0011
	ULCON1 = 3;
	
	// 3. set the UCONn
	// clock is: PCLK
	// Interrupt type is Level
	// Disable time-out interrupt
	// Do not generate error status interrupt
	// Transmit/Receive mode is interrupt or polling
	// 1011_0000_0101
	UCON1 = (UCON1 & ~0xFFF) | 0xB05;
	
	// disable FIFO
	UFCON1 = 0;
	
	// set the UMCONn to disable flow control
	UMCON1 = 0;
	
	// enable UART 1 source interrupt(RXD)
    UINTM1 &= ~1;
	
	//--------------------------------------------------------------------------
	
	// VIC0 0-31
	// VIC1 32-63
	// INT_UART1 interrupt is in No.38, VIC1
	VIC1INTENABLE |= (1 << 6);
	
	// all the interrupt is set to irq in default
	
	// ISR address
	VIC1VECTADDR[6] = (unsigned int) uart_1_isr;
    
	return;
}

void uart_1_putc(unsigned char ch)
{
    while (1) {
		if ((UTRSTAT1 & 6) == 6) break;
	}
	UTXH1 = ch;
	
	return;
}

void uart_1_puts(char *str)
{
    for (int i = 0; str[i] != '\0'; ++i) {
		uart_1_putc(str[i]);
	}
    
    return;
}

void uart_1_printf(char *str)
{
    for (int i = 0; str[i] != '\0'; ++i) {
		uart_1_putc(str[i]);
	}
    
    return;
}


unsigned char uart_1_getc(void)
{
    while (!receive_over);
    
    receive_over = false;
    
    return uart1_buffer[0];
}


void uart_1_gets(void)
{
    while (!receive_over);
    
    receive_over = false;
    
    return;
}



// new line is \r\n -> 0x0D 0x0A
void uart_1_isr(void)
{
    if (UTRSTAT1 & 1) {
        
        if (AT_MODE) {
            uart_putc(URXH1);
        }
        
        if (!AT_MODE) {
            uart1_buffer[buffer_index] = URXH1;
            
            if (buffer_index >= 1 && uart1_buffer[buffer_index-1] == '\r' &&
                uart1_buffer[buffer_index] == '\n') {
                
                uart1_buffer[buffer_index+1] = '\0';
                receive_over = true;
                buffer_index = 0;
                
                uart_printf(uart1_buffer);
                receive_over = false;
            } else {
                ++buffer_index;
            }
        }
        
    }
    
    //clear interrupt
    UINTP1 = 0xF;
    VIC1ADDRESS = 0;
    
    return;
}





