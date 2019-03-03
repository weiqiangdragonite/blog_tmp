/******************************************************************************
* OK6410
* For GNU ASM
*
* init.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-13
******************************************************************************/


		.text
_start: .global _start


/*****************************************************************************/


START:

		@ 0x00000000 复位RESET (SVC模式) 异常向量地址
		B			RESET
		
		@ 0x00000004 未定义指令UND (UND模式) 异常向量地址
HANDLE_UND:
		LDR			PC, =HANDLE_UND
		
		@ 0x00000008 软件中断SWI (SVC模式) 异常向量地址
HANDLE_SWI:
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

		@ 关闭总中断
		MRS			R0, CPSR
		ORR			R0, R0, #0xC0					@ 第6、7位设为1，禁止IRQ和FIQ中断
		MSR			CPSR_c, R0

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
		BL          INIT_DDR
		
        @ 设置栈指针，因为DDR内存地址为0x50000000 ~ 0x5FFFFFFF，
        @ 设栈底为0x5FFFFFF0
		@ 7种模式下的栈大小各为4KB
.equ	STACK_START, 0x5FFFFFF0
.equ	STACK_SIZE,  4 * 1024
        
		@ 设置SVC模式下的栈指针
		LDR			R0, =STACK_START
		MOV			SP, R0
		SUB			R0, R0, #STACK_SIZE
		
		@ 设置ABT模式下的栈指针
		MSR			CPSR_c, #0xD7
		MOV			SP, R0
		SUB			R0, R0, #STACK_SIZE
		
		@ 设置UND模式下的栈指针
		MSR			CPSR_c, #0xDB
		MOV			SP, R0
		SUB			R0, R0, #STACK_SIZE
		
		@ 设置IRQ模式下的栈指针
		MSR			CPSR_c, #0xD2
		MOV			SP, R0
		SUB			R0, R0, #STACK_SIZE
		
		@ 设置FIQ模式下的栈指针
		MSR			CPSR_c, #0xD1
		MOV			SP, R0
		SUB			R0, R0, #STACK_SIZE
		
		@ 设置USR/SYS模式下的栈指针
		MSR			CPSR_c, #0xDF
		MOV			SP, R0
		
		@ 返回SVC模式
		MSR			CPSR_c, #0xD3
		
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


		@ 跳到DDR的连接地址，开始执行代码
		LDR			PC, =CODE_START
CODE_START:

		@ 开启CPU总中断
		MRS			R0, CPSR						@ 将CPSR保存至R0寄存器中
		BIC			R0, R0, #0x80					@ 第7位设为0，允许IRQ中断
		MSR			CPSR_c, R0
		
		@ 开启向量中断VECT
		@MRC			p15, 0, R0, c1, c0, 0
		@ORR			R0, R0, #(1<<24)				@>
		@MCR			p15, 0, R0, c1, c0, 0

		@ 跳到main函数
		BL			main
		
/*****************************************************************************/

HANDLE_IRQ:

		@ 保存上下文
		SUB			LR, LR, #4
		STMFD		SP!, {R0-R12, LR}
		
		@ 处理异常
		BL			whichISR
		
HANDLE_IRQ_END:
		@ 恢复上下文
		LDMFD		SP!, {R0-R12, PC}^

/*****************************************************************************/
		NOP
		NOP
		.end

