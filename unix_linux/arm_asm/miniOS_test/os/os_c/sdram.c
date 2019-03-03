/******************************************************************************
* File: sdram.c
* 
* Description:
* 	This function init the sdram.
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-26
******************************************************************************/

#include "registers.h"
#include "uart.h"


void init_sdram(void)
{

	/** set DRAM controller sequence*/
	
	// 1. set P1MEMCCMD to Configure
	P1MEMCCMD = 4;
	
	// 2.
	// set the refresh period
	P1REFRESH = 1037;
	
	// CAS Latency
	P1CASLAT = 6;
	
	// DQSS
	P1T_DQSS = 1;
	
	// MRD
	P1T_MRD = 2;
	
	// RAS
	P1T_RAS = 7;
	
	// RC
	P1T_RC = 10;
	
	// RCD
	P1T_RCD = (1 << 3) | 4;

	// RFC
	P1T_RFC = (8 << 5) | 11;
	
	// RP
	P1T_RP = (1 << 3) | 4;
	
	// RRD
	P1T_RRD = 3;
	
	// WR
	P1T_WR = 3;
	
	// WTR
	P1T_WTR = 2;
	
	// XP
	P1T_XP = 1;
	
	// XSR
	P1T_XSR = 17;
	
	// ESR
	P1T_ESR = 17;
	
	// 3. set the P1MEMCFG register
	// set the [5:0] to 011010
	P1MEMCFG = (P1MEMCFG & 0xFFFFFFC0) | 0x1A;
	
	// 4. set the P1MEMCFG2 register
	// set the [12:0] to 0_1011_0100_0101
	P1MEMCFG2 = 0xB45;
	
	// 5. set the P1_chip_0_cfg register
	// set the [16] to Bank-Row-Column organization
	P1CHIP0CFG |= (1 << 16);
    
	
	/** set the SDRAM sequence */
	
	// 6. set P1DIRECTCMD:
	// to NOP
	P1DIRECTCMD = 0xC0000;
	// to PrechargeAll
	P1DIRECTCMD = 0;
	// to Autorefresh
	P1DIRECTCMD = 0x40000;
	// to Autorefresh
	P1DIRECTCMD = 0x40000;
	// to MRS
	P1DIRECTCMD = 0x80032;
	// t0 EMRS
	P1DIRECTCMD = 0xA0000;
	
	// 7. set MEM_SYS_CFG[5:0] to NFCON
	MEM_SYS_CFG = 0;
	
	// 8. set P1MEMCCMD to Go
	P1MEMCCMD = 0;
	
	// 9. wait P1MEMSTAT to become Ready
	while (1) {
		if ((P1MEMSTAT & 0x3) == 1) break;
	}
    
	return;
}


void copy_to_sdram(void)
{
    
    // 将第二部分代码复制到SDRAM中。因为第二部分代码的加载地址在连接时
    // 被指定为4096(4KB)之后，系统启动后，把代码复制到内存中0x5000_4000处，
    // 虚拟地址为0xB000_4000，复制的数据
    
    unsigned int *dest = (unsigned int *) 0x50000000;
    unsigned int *start = (unsigned int *) 0;
    unsigned int *end = (unsigned int *) (8 * 1024);
    
    while (start < end) {
        *(dest++) = *(start++);
    }
    
    return;
}


void sdram_read(unsigned int start, unsigned int end)
{
    unsigned int *start_addr = (unsigned int *) start;
    
    int column = 0;
    while (1) {
        if (start >= end) break;
        
        if (column == 0 || column == 4) {
            uart_puts("");
            uart_print_hex(start);
            uart_printf(" : ");
            column = 0;
        }
        
        uart_print_addr(*start_addr++);
        uart_printf("  ");
        ++column;
        start += 4;
    }
    uart_puts("");
    
    return;
}
