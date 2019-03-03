/******************************************************************************
* OK6410 Exception
* For GNU ASM
*
* init.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-12
******************************************************************************/


		.text
_start: .global _start


/*****************************************************************************/


START:

		@ 0x00000000 复位RESET (SVC模式) 异常向量地址
		B			RESET
		
		@ 0x00000004 未定义指令UND (UND模式) 异常向量地址
		LDR			PC, =HANDLE_UND
		
		@ 0x00000008 软件中断SWI (SVC模式) 异常向量地址
		LDR			PC, =HANDLE_SWI
		
		@ 0x0000000C 指令预取中止PABT (ABT模式) 异常向量地址
HANDLE_PABT:
		LDR			PC, =HANDLE_PABT
		
		@ 0x00000010 数据访问中止DABT (ABT模式) 异常向量地址
HANDLE_DABT:
		LDR			PC, =HANDLE_DABT
		
		@ 0x00000014 系统保留
RESERVED:
		B			RESERVED
		
		@ 0x00000018 外部中断请求IRQ (IRQ模式) 异常向量地址
HANDLE_IRQ:
		LDR			PC, =HANDLE_IRQ
		
		@ 0x0000001C 快速中断请求FIQ (FIQ模式) 异常向量地址
HANDLE_FIQ:
		LDR			PC, =HANDLE_FIQ
		
		@ 快速中断FIQ的处理程序可以不用设置跳转，直接从这里添加处理程序


/*****************************************************************************/


RESET:

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
		CMP			R0, R1							@ 看CODE_START是否已经处于连接地址
		BEQ			CLEAN_BSS						@ 若处于连接地址，跳到BSS段清零处
		
		@ 复制SRAM的代码到DDR中
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


/*****************************************************************************/


@ define Buzzer GPIO address
.equ	GPFCON, 0x7F0080A0
.equ	GPFDAT, 0x7F0080A4
.equ	GPFPUD, 0x7F0080A8

		
		@ 跳到连接地址，开始执行代码
		LDR			PC, =CODE_START
		@B			CODE_START
CODE_START:

		BL			INIT_BUZZER
		
		SWI			0
		
		BL			DELAY
		BL			DELAY
		
		SWI			0
		BL			DELAY
		
ERR_UND:
		.word		0xFF000000
		
		BL			DELAY
		
HALT:
		B			HALT
		
		
/*****************************************************************************/


INIT_BUZZER:

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
		
INIT_BUZZER_END:
		MOV			PC, LR


/*****************************************************************************/


HANDLE_SWI:

		@ 保存上下文
		STMFD		SP!, {R0-R12, LR}
		
		@ 处理异常
		BL			RING_BUZZER
		
HANDLE_SWI_END:
		@ 恢复上下文
		LDMFD		SP!, {R0-R12, PC}^


/*****************************************************************************/


HANDLE_UND:

		@ 设置栈指针
		LDR			SP, =0x55000000
		
		@ 保存上下文
		STMFD		SP!, {R0-R12, LR}
		
		@ 处理异常
		BL			RING_BUZZER


HANDLE_UND_END:
		@ 恢复上下文
		LDMFD		SP!, {R0-R12, PC}^


/*****************************************************************************/


RING_BUZZER:

		@ 保存上下文
		STMFD		SP!, {R0-R12, LR}

        @ 设置GPFDAT
		LDR			R0, =GPFDAT
		LDR			R1, [R0]
		
        @ 蜂鸣器响
		LDR			R2, =0x00008000					@ 共16位，最高位为1
		ORR			R1, R2
		STR			R1, [R0]
		
        @ 延时
		BL			DELAY
		BL			DELAY
		
        @ 停
		LDR			R2, =0xFFFF7FFF
		AND			R1, R2
		STR			R1, [R0]
		
RING_BUZZER_END:
		@ 恢复上下文
		LDMFD		SP!, {R0-R12, LR}
		MOV			PC, LR


/****************************************************************************/


DELAY:
		MOV			R4, #0
		LDR			R5, =0x004FFFFF
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

