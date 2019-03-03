/******************************************************************************
* OK6410 Mobile DDR SDRAM
* For GNU ASM
*
* init.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-9
******************************************************************************/


		.text
_start: .global _start

		
/*****************************************************************************/

@ define Buzzer GPIO address
.equ	GPFCON, 0x7F0080A0
.equ	GPFDAT, 0x7F0080A4
.equ	GPFPUD, 0x7F0080A8

START:

        @ 设置寄存器地址
		LDR			R0, =0x70000000
		ORR			R0, R0, #0x00000013
		MCR			p15, 0, R0, c15, c2, 4
		
        @ 关闭看门狗
		LDR			R0, =0x7E004000
		MOV			R1, #0
		STR			R1, [R0]

        @ 初始化时钟
		BL			INIT_CLOCK
		
		@ 初始化DDR
		BL			INIT_DDR
		
        @ 设置栈，应为CPU会把NAND前8K复制到SRAM的Steppingstone内，所以设置
        @ 栈底为
        @MOV         SP,

		@ 重定位代码
		ADR			R0, START						@ START地址为0
		LDR			R1, =START						@ START的连接地址
		LDR			R2, =bss_start					@ bss_start的开始地址
		CMP			R0, R1							@ 看CODE_START是否已经处于连接地址
		BEQ			CLEAN_BSS						@ 若处于连接地址，跳到BSS段清零处
		
COPY_CODE:											@ 不处于连接地址，开始复制代码到连接地址处
		LDR			R3, [R0], #4					@ R3 <- [R0], R0 <- R0 + 4
		STR			R3, [R1], #4					@ [R1] <- R3, R1 <- R1 + 4
		CMP			R1, R2
		BNE			COPY_CODE
		
CLEAN_BSS:											@ bss段清零
		LDR			R0, =bss_start
		LDR			R1, =bss_end
		MOV			R2, #0
CLEAN_LOOP:
        CMP         R0, R1
		STRNE       R2, [R0], #4
		BNE			CLEAN_LOOP
		
		
		@ 跳到连接地址，开始执行代码
		LDR			PC, =CODE_START
CODE_START:
		@ 设置GPFCON，引脚设为输出模式
		LDR			R0, =GPFCON
		LDR			R1, [R0]
		LDR			R2, =0x3FFFFFFF
		AND			R1, R2							@ 高2位清零
		LDR			R2, =0x40000000
		ORR			R1, R2							@ 高2位设为01
		STR			R1, [R0]
		
        @ 设置GPFPUD，上下拉电阻禁止
		LDR			R0, =GPFPUD
		LDR			R1, [R0]
		LDR			R2, =0x3FFFFFFF
		AND			R1, R2							@ 高2位清零
		STR			R1, [R0]
		
/*****************************************************************************/
		
        @ 设置GPFDAT
		LDR			R0, =GPFDAT
		LDR			R1, [R0]
LOOP:
        @ 蜂鸣器响
		LDR			R2, =0x00008000					@ 共16位，最高位为1
		ORR			R1, R2
		STR			R1, [R0]
		
        @ 延时
		BL			DELAY
		
        @ 停
		LDR			R2, =0xFFFF7FFF
		AND			R1, R2
		STR			R1, [R0]
		
        @ 延时
		BL			DELAY
		
		B			LOOP
		
@CODE_END

/****************************************************************************/

DELAY:
		MOV			R4, #0
		LDR			R5, =0x0004FFFF
DELAY_START:
		CMP			R4, R5
		BEQ			DELAY_END
		ADD			R4, R4, #1
		B			DELAY_START
DELAY_END:
		MOV			PC, LR

/*****************************************************************************/
		NOP
		NOP
		.end

