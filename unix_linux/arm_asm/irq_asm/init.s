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
@ define key GPIO address
.equ	GPNCON, 0x7F008830
.equ	GPNDAT, 0x7F008834
.equ	GPNPUD, 0x7F008838
@
.equ	EINT0CON0, 0x7F008900
.equ	EINT0MASK, 0x7F008920
.equ	EINT0PEND, 0x7F008924
.equ	VIC0INTENABLE, 0x71200010
.equ	VIC0ADDRESS, 0x71200F00

		
		@ 跳到连接地址，开始执行代码
		LDR			PC, =CODE_START
		@B			CODE_START
CODE_START:

		BL			INIT_BUZZER
		BL			INIT_INT
		
		@ 打开IRQ中断总开关
		MRS			R0, CPSR						@ 将CPSR保存至R0寄存器中
		BIC			R0, R0, #0x80					@ 第7位设为0，允许IRQ中断
		MSR			CPSR_c, R0
		
		
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


INIT_INT:

		@ 首先设置6个按键的GPNCON为外部中断模式，即10
		LDR			R0, =GPNCON
		LDR			R1, [R0]
		LDR			R2, =0xFFFFF000
		AND			R1, R2							@ 低12位设为0
		LDR			R2, =0x00000AAA
		ORR			R1, R2							@ 低12位设为1010 1010 1010
		STR			R1, [R0]
		
		@ 设置GPNPUD上下拉电阻为禁止
		LDR			R0, =GPNPUD
		LDR			R1, [R0]
		LDR			R2, =0xFFFFF000
		AND			R1, R2
		STR			R1, [R0]
		
		@ 设置中断信号类型位双边沿触发
		LDR			R0, =EINT0CON0
		LDR			R1, [R0]
		LDR			R2, =0xFFFFF000
		AND			R1, R2							@ 低12位设为0
		LDR			R2, =0x00000777
		ORR			R1, R2							@ 低12位为
		STR			R1, [R0]
		
		@ 在中断源中使能中断，设置屏蔽中断寄存器，取消屏蔽，低6位设为0
		LDR			R0, =EINT0MASK
		LDR			R1, [R0]
		LDR			R2, =0xFFFFFFC0
		AND			R1, R2
		STR			R1, [R0]
		
		@ 在中断控制器中使能这些中断
		@ 设置中断使能寄存器，打开1号、2号中断，即最低2位为11
		LDR			R0, =VIC0INTENABLE
		LDR			R1, [R0]
		ORR			R1, #3
		STR			R1, [R0]
		

INIT_INT_END:
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


HANDLE_IRQ:

		@ 设置栈指针
		LDR			SP, =0x55000000
		
		@ 保存上下文
		SUB			LR, LR, #4
		STMFD		SP!, {R0-R12, LR}
		
		@ 处理中断
		@ 分辨中断源
		LDR			R0, =EINT0PEND
		LDR			R1, [R0]
		AND			R1, #1
		@ 判断是不是按键1产生中断
		CMP			R1, #1
		BNE			CLEAN_IRQ
		@ 判断是不是按键1松开
		LDR			R0, =GPNDAT
		LDR			R1, [R0]
		AND			R1, #1
		CMP			R1, #1
		@ 设置GPFDAT
		LDR			R0, =GPFDAT
		LDR			R1, [R0]
        @ 蜂鸣器响
		LDRNE		R2, =0x00008000					@ 共16位，最高位为1
		ORRNE		R1, R2
		LDREQ		R2, =0xFFFF7FFF
		ANDEQ		R1, R2
		STR			R1, [R0]
		
		@ 清除中断
CLEAN_IRQ:
		@ 清除中断源
		LDR			R0, =EINT0PEND
		MOV			R1, #0x3F
		STR			R1, [R0]
		@ 清除中断控制寄存器
		LDR			R0, =VIC0ADDRESS
		MOV			R1, #0
		STR			R1, [R0]
		

HANDLE_IRQ_END:
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

