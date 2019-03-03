

#define APLL_LOCK	0x7e00f000
#define MPLL_LOCK	0x7e00f004
#define EPLL_LOCK	0x7e00f008
#define LOCK_TIME	0xffff

#define OTHERS		0x7e00f900

#define CLK_DIV0		0x7e00f020

#define CLK_SRC		0x7e00f01c

	.text
	.global clock_init
clock_init:

	@ set the lock time to max
	ldr r0, =LOCK_TIME
	ldr r1, =APLL_LOCK
	str r0, [r1]
	ldr r1, =MPLL_LOCK
	str r0, [r1]
	ldr r1, =EPLL_LOCK
	str r0, [r1]	
					
	@ set async mode
	ldr r0, =OTHERS
	ldr r1, [r0]
	bic r1, #0xc0			
	str r1, [r0]

	loop1:
	ldr r0, =OTHERS
	ldr r1, [r0]
	and r1, #0xf00					
	cmp r1, #0
	bne loop1		

	@ set the divider

	#define DIV_VAL	( (0)|(1<<4)|(1<<8)|(1<<9)|(3<<12) )
	ldr r0, =CLK_DIV0
	ldr r1, =DIV_VAL
	str r1, [r0]	
			
	@ set APLL, MPLL, EPLL		
	#define SDIV	1
	#define PDIV	3
	#define MDIV	266	
	#define PLL_ENABLE	( 1 << 31 )
	#define APLL_VAL	( (SDIV<<0)|(PDIV<<8)|(MDIV<<16)|(PLL_ENABLE) )
	#define MPLL_VAL	APLL_VAL
	#define EPLL0_VAL	( (2<<0)|(1<<8)|(32<<16)|PLL_ENABLE)
	#define EPLL1_VAL	( 0 )
								
	#define APLL_CON	0x7e00f00c
	#define MPLL_CON	0x7e00f010
	#define EPLL_CON0	0x7e00f014
	#define EPLL_CON1	0x7e00f018

	ldr r0, =APLL_CON
	ldr r1, =APLL_VAL
	str r1, [r0]

	ldr r0, =MPLL_CON
	ldr r1, =MPLL_VAL
	str r1, [r0]
	
	ldr r0, =EPLL_CON0
	ldr r1, =EPLL0_VAL
	str r1, [r0]

	ldr r0, =EPLL_CON1
	ldr r1, =EPLL1_VAL
	str r1, [r0]

	@ select the source 					
	ldr r0, =CLK_SRC
	mov r1, #7
	str r1, [r0]

	mov pc, lr				
	

