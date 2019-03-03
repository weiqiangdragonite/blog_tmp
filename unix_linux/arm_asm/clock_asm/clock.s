/******************************************************************************
* OK6410 Clock & Buzzer
* For GNU ASM
*
* clock.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-9
******************************************************************************/


        .global INIT_CLOCK

/*****************************************************************************/

@ clock register address
.equ	APLL_LOCK,	0x7E00F000
.equ	MPLL_LOCK,	0x7E00F004
.equ	EPLL_LOCK,	0x7E00F008
.equ	OTHERS,		0x7E00F900
.equ	APLL_CON,	0x7E00F00C
.equ	MPLL_CON,	0x7E00F010
.equ	EPLL_CON0,	0x7E00F014
.equ	EPLL_CON1,	0x7E00F018
.equ	CLK_DIV0,	0x7E00F020
.equ	CLK_SRC,	0x7E00F01C


INIT_CLOCK:

		@ 1. set the lock time
		@ the xPLL lock time set to default 0x0000FFFF
		LDR			R0, =APLL_LOCK
		LDR			R1, =0x0000FFFF
		STR			R1, [R0], #4				@ [R0] <- R1, R0 <- R0 + 4; set the APLL
		STR			R1, [R0], #4				@ set the MPLL
		STR			R1, [R0]					@ set the EPLL
		
		@ 2. set the others control registers
		@ set SYNMODE to Asynchronous mode (When the ARMCLK != HCLK, we need to set this!)
		@ and we need to set SYNCMUXSEL to DOUTMPLL
		LDR			R0, =OTHERS
		LDR			R1, [R0]
		BIC			R1, #0x000000C0
		STR			R1, [R0]
		
		@ 3. wait for the system colck mode to be asynchronous mode
		@ get the SYNC mode from OTHERS register
LOOP:
		LDR			R0, =OTHERS
		LDR			R1, [R0]
		@ 读出8~11位
		AND			R1, #0x00000F00
		CMP			R1, #0
		BNE LOOP
		
		@ 4. set the Fout
		@ set the APLL Fout
		@ Fout(APLL) = 12 * 266 / (3 * 2 ^ 1) = 532 MHz
		@ 1-xxxxx-MDIV分频值-xx-PDIV分频值-xxxxx-SDIV分频值
		@ 1-00000-0100001010-00-000011-00000-001
		LDR			R0, =APLL_CON
		LDR			R1, =((1 << 31) | (266 << 16) | (3 << 8) | 1)
		STR			R1, [R0]
		
		@ set the MPLL Fout
		@ Fout(MPLL) = 12 * 266 / (3 * 2 ^ 2) = 266 MHz
		@ 1-xxxxx-MDIV分频值-xx-PDIV分频值-xxxxx-SDIV分频值
		@ 1-00000-0100001010-00-000011-00000-010
		LDR			R1, =((1 << 31) | (266 << 16) | (3 << 8) | 2)
		STR			R1, [R0, #4]
		
		@ set the EPLL Fout
		@ ...
		
		@ 5. set the clock divider
		@ ARM_RATIO = 0, MPLL_RATIO = 0, HCLKX2_RATIO = 0,
		@ HCLK_RATIO = 1, PCLK_RATIO = 3
		@ 0011_000 1_000 0_0000
		LDR			R0, =CLK_DIV0
		LDR			R1, [R0]
		LDR			R2, =0xFFFF0000
		AND			R1, R2
		LDR			R2, =0x00003100
		ORR			R1, R2
		STR			R1, [R0]
		
		@ 6. set the clock source
		@ set to use APLL and MPLL
		LDR			R0, =CLK_SRC
		LDR			R1, [R0]
		ORR			R1, #0x00000003
		STR			R1, [R0]
		

/*****************************************************************************/
        MOV         PC, LR

