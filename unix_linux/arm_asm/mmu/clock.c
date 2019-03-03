/******************************************************************************
* File: clock.c
* 
* Description:
*	This function init the system clock to:
*	APLL - 532 MHz, MPLL - 266 MHz, ARMCLK - 532 MHz, HCLKX2 - 266 MHz
*	HCLK - 133 MHz, PCLK - 66  MHz, JPEG   - 66  MHz, SECUR  - 66  MHz
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-13
******************************************************************************/


/* define the clock register */
#define		APLL_LOCK		(*(volatile unsigned int *) (0x7E00F000))
#define		MPLL_LOCK		(*(volatile unsigned int *) (0x7E00F004))
#define		EPLL_LOCK		(*(volatile unsigned int *) (0x7E00F008))
#define		OTHERS			(*(volatile unsigned int *) (0x7E00F900))
#define		APLL_CON		(*(volatile unsigned int *) (0x7E00F00C))
#define		MPLL_CON		(*(volatile unsigned int *) (0x7E00F010))
#define		EPLL_CON0		(*(volatile unsigned int *) (0x7E00F014))
#define		EPLL_CON1		(*(volatile unsigned int *) (0x7E00F018))
#define		CLK_DIV0		(*(volatile unsigned int *) (0x7E00F020))
#define		CLK_SRC			(*(volatile unsigned int *) (0x7E00F01C))


void initClock(void)
{

	/** 1. set the lock time */
	// the xPLL lock time set to default 0x0000FFFF
	APLL_LOCK = 0x0000FFFF;
	MPLL_LOCK = 0x0000FFFF;
	EPLL_LOCK = 0x0000FFFF;
	

	/** 2. set the others control registers */
	// set SYNMODE to Asynchronous mode (When the ARMCLK != HCLK, we need to set this!)
	// and we need to set SYNCMUXSEL to DOUTMPLL
	OTHERS &= 0xFFFFFF3F;
	
	
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
	// ARM_RATIO = 0, MPLL_RATIO = 0, HCLKX2_RATIO = 0,
	// HCLK_RATIO = 1, PCLK_RATIO = 3
	// 0011_000 1_000 0_0000
	CLK_DIV0 = (CLK_DIV0 & 0xFFFF0000) | 0x00003100;
	
	
	/** 6. set the clock source */
	// set to use APLL and MPLL
	CLK_SRC = 3;
	

	return;
}
