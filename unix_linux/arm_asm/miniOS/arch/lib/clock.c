/******************************************************************************
* File: clock.c
* 
* Description:
*	The default system clock is set to:
*	APLL - 532 MHz, MPLL - 266 MHz, ARMCLK  - 532 MHz, HCLKX2   - 266 MHz
*	HCLK - 133 MHz, PCLK - 66  MHz, CLKJPEG -  66 MHz, CLKSECUR -  66 MHz
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-26
******************************************************************************/

/*

Typical value setting for clock dividers
APLL    MPLL    ARMCLK    HCLKX2    HCLK    PCLK    CLKJPEG    CLKSECUR
266     266     0/266     0/266     1/133   3/66    3/66       3/66
400     266     0/400     0/266     1/133   3/66    3/66       3/66
533     266     0/533     0/266     1/133   3/66    3/66       3/66
667     266     0/667     0/266     1/133   3/66    3/66       3/66

APLL/MPLL: ENABLE[31] MDIV[25:16] PDIV[13:8] SDIV[2:0]
Fout = MDIV * Fin / (PDIV * (2 ^ SDIV))
       266 * 12 / (3 * (2 ^ 2)) = 266 MHz
       400 * 12 / (3 * (2 ^ 2)) = 400 MHz
       266 * 12 / (3 * (2 ^ 1)) = 532 MHz
       333 * 12 / (3 * (2 ^ 1)) = 666 MHz
       
EPLL_CON0: ENABLE[31] MDIV[23:16] PDIV[13:8] SDIV[2:0]
EPLL_CON1: KDIV[15:0]
Fout = (MDIV + KDIV / (2 ^ 16)) * Fin / (PDIV * (2 ^ SDIV))

*/


#include "registers.h"


void init_clock(void)
{

	/** 1. set the PLL lock time */
	// the xPLL lock time is set to default 0x0000FFFF
	APLL_LOCK = 0x0000FFFF;
	MPLL_LOCK = 0x0000FFFF;
	EPLL_LOCK = 0x0000FFFF;
	

	/** 2. set the others control registers */
	// set SYNMODE to Asynchronous mode
    // (When the ARMCLK != HCLK, we need to set this!)
	// and set the SYNCMUXSEL to use DOUTMPLL
	OTHERS &= 0xFFFFFF3F;
    //OTHERS &= ~0xC;
	
	
	/** 3. wait for the system colck mode to be asynchronous mode */
	// get the SYNC mode from OTHERS register
	while (1) {
		if ((OTHERS & 0x00000F00) == 0) break;
	}
	
	
	/** 4. set the Fout */
	// set the APLL Fout
	// Fout(APLL) = 12 * 266 / (3 * 2 ^ 1) = 532 MHz
	APLL_CON = (1 << 31) | (266 << 16) | (3 << 8) | 1;
	
	// set the MPLL Fout
	// Fout(MPLL) = 12 * 266 / (3 * 2 ^ 2) = 266 MHz
	MPLL_CON = (1 << 31) | (266 << 16) | (3 << 8) | 2;
	
	// set the EPLL Fout
	// 
	
	
	/** 5. set the clock divider */
	// ARM_RATIO = 0, MPLL_RATIO = 0, HCLKX2_RATIO = 0, HCLK_RATIO = 1
	// PCLK_RATIO = 3, SECUR_RATIO = 3, JPEG_RATIO = 3
	// XXXX_0011_XXXX_11XX_0011_0001_0000_0000
	CLK_DIV0 = (CLK_DIV0 & 0xF0F30000) | 0x030C3100;
	
	
	/** 6. set the clock source */
	// set to use APLL and MPLL
	CLK_SRC = 3;
    

	return;
}
