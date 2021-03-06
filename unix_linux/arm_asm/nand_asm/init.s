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
        @ 栈底为8k
        LDR			SP, =8 * 1024

		@ 重定位代码
		ADR			R0, START						@ START地址为0
		LDR			R1, =START						@ START的连接地址
		LDR			R2, =bss_start					@ bss_start的开始地址
		SUB			R2, R2, R1						@ 得到代码段和数据段的长度
		
		CMP			R0, R1							@ 比较START的当前地址和连接地址
		BEQ			CLEAN_BSS						@ 若相等跳到清BSS段
		
		@ 把NAND Flash的程序复制到DDR，参数为R0, R1, R2
		BL			COPY_NAND
		
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
		@B			CODE_START
CODE_START:


		BL			INIT_UART
		LDR			R0, =0x50000000
		LDR			R1, =START
		LDR			R2, =bss_start
		SUB			R2, R2, R1
		MOV			R7, R2
		
		MOV			R6, #0
NAND_DATA:
		LDR			R1, [R0], #4
		BL			SEND_DATA
		ADD			R6, R6, #4
		CMP			R6, R7
		BNE			NAND_DATA


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

