/******************************************************************************
* OK6410 NAND FLASH
* For GNU ASM
*
* nand.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-11
******************************************************************************/


		.global COPY_NAND
		
/*****************************************************************************/

.equ	NFCONF,		0x70200000
.equ	NFCONT,		0x70200004
.equ	NFCMMD,		0x70200008
.equ	NFSTAT,		0x70200028
.equ	NFADDR,		0x7020000C
.equ	NFDATA,		0x70200010

COPY_NAND:

		@ 传递的参数R0, R1, R2如下
		@ R0 = 开始复制的地址
		@ R1 = 传送到的地址
		@ R2 = 复制的长度（字节）
		@ 这里不用栈，把R0 ~ R2 存放到 R6 ~ R8
		MOV			R6, R0
		MOV			R7, R1
		MOV			R8, R2
		
		
		@MOV			R12, LR
		@BL			INIT_UART
		@MOV			LR, R12
		
		/*********************************************************************/

		@ 初始化NAND
		
		@ 1. 设置MEM_SYS_CFG为NAND引导
		LDR			R0, =0x7E00F120
		MOV			R1, #0
		STR			R1, [R0]
		
		@ 2. 设置NFCONF参数
		@ TACLS = 2; TWRPH0 = 1; TWRPH1 = 0  <-- 这个参数不行
		@ TACLS = 0; TWRPH0 = 1; TWRPH1 = 0  <-- 这个参数不行
		@ TACLS = 0; TWRPH0 = 3; TWRPH1 = 1  <-- OK
		@ TACLS = 5; TWRPH0 = 5; TWRPH1 = 5  <-- OK
		@ 应该是时序（周期）的问题
		LDR			R0, =NFCONF
		LDR			R1, [R0]
		LDR			R2, =0xBE7F888F
		AND			R1, R2
		LDR			R2, =0x01000310
		ORR			R1, R2
		STR			R1, [R0]
		
		
		@ 3. 使能NAND Flash控制寄存器
		LDR			R0, =NFCONT
		LDR			R1, [R0]
		ORR			R1, #1
		STR			R1, [R0]
		
		/*********************************************************************/
		
		@ 复位NAND
		
		@ 1. 选中NAND Flash芯片
		LDR			R0, =NFCONT
		LDR			R1, [R0]
		BIC			R1, #2
		STR			R1, [R0]
		
		@ 2. 发出复位Reset命令
		LDR			R0, =NFCMMD
		MOV			R1, #0xFF
		STR			R1, [R0]
		
		@ 3. 等待NAND Flash就绪
		LDR			R0, =NFSTAT
WAIT_READY1:
		LDR			R1, [R0]
		AND			R1, #1
		CMP			R1, #0
		BEQ			WAIT_READY1
		
		@ 4. 取消选中NAND
		LDR			R0, =NFCONT
		LDR			R1, [R0]
		ORR			R1, #2
		STR			R1, [R0]
		
		/*********************************************************************/
		
		@ 读取NAND数据
		
		@ 1. 选中NAND Flash芯片
		LDR			R0, =NFCONT
		LDR			R1, [R0]
		BIC			R1, #2
		STR			R1, [R0]
		
		@ 开始地址为R6（SRAM中的0x00000000地址）
		@ 传送到DDR中的地址为R7（0x50000000）
		@ 复制的长度为R8
		@ R9为已经复制的长度
		MOV			R9, #0

WHILE_LOOP:
		@ 2. 发出读命令 - 0x00
		LDR			R0, =NFCMMD
		MOV			R1, #0
		STR			R1, [R0]
		
		@ 3. 发出开始读取数据的地址
		/*
		@ 在NAND Flash中取出某地址中的数据，需要行和列地址
		@ 因为是采用页管理，1页的大小为4 KB，即4096 Bytes，0x1000
		@ 所以比如0x2050的地址，需要计算出在NAND中的哪一页，哪一列
		@ 0x2050 / 0x1000 = 0x2, 0x2050 - 0x2 * 0x1000 = 0x50
		@ 即第0x2页，第0x50列
		@ 先发列地址，分两个周期发
		@ 在发页地址， 分三个周期发
		LDR			R0, =NFADDR
		@ 页大小为0x1000，保存在R1中
		LDR			R1, =0x1000
		@ 计算页地址，保存在R2中
		MUL			R2, R6, R1
		@ 计算列地址，保存在R3中
		MUL			R3, R2, R1
		SUB			R3, R6, R3
		
		@ 发送列地址，R4作临时寄存器
		MOV			R4, R3
		AND			R4, #0xFF
		STR			R4, [R0]
		MOV			R4, R3
		MOV			R4, R4, LSR #8
		AND			R4, #0xFF
		STR			R4, [R0]
		
		@ 发送页地址，R4作临时寄存器
		MOV			R4, R2
		AND			R4, #0xFF
		STR			R4, [R0]
		MOV			R4, R2
		MOV			R4, R4, LSR #8
		AND			R4, #0xFF
		STR			R4, [R0]
		MOV			R4, R2
		MOV			R4, R4, LSR #16
		AND			R4, #0xFF
		STR			R4, [R0]
		*/
		
		@ R6 为开始地址
		LDR			R0, =NFADDR
		MOV			R1, R6
		@ 取低8位0 ~ 7
		AND			R1, #0xFF
		STRB		R1, [R0]
		
		MOV			R1, R6
		@ 取8 ~ 12位，并右移至最低位
		MOV			R1, R1, LSR #8
		AND			R1, #0x1F
		STRB		R1, [R0]
		
		MOV			R1, R6
		@ 取位13 ~ 20
		MOV			R1, R1, LSR #13
		AND			R1, #0xFF
		STRB		R1, [R0]
		
		MOV			R1, R6
		@ 取位21 ~ 28
		MOV			R1, R1, LSR #21
		AND			R1, #0xFF
		STRB		R1, [R0]
		
		MOV			R1, R6
		@ 取位29 ~ 31
		MOV			R1, R1, LSR #29
		AND			R1, #0x07
		STRB		R1, [R0]
		
		
		@ 4. 发出开始读命令 - 0x30
		LDR			R0, =NFCMMD
		MOV			R1, #0x30
		STR			R1, [R0]
		
		@ 5. 等待NAND Flash就绪
		LDR			R0, =NFSTAT
WAIT_READY2:
		LDR			R1, [R0]
		AND			R1, #1
		CMP			R1, #0
		BEQ			WAIT_READY2
		
		@ 6. 开始读取数据
		@ 因为一次返回一页数据4 KB，即4096字节，每次读取4字节
		@ 共需要读取1024次，即0x400
		LDR			R0, =NFDATA
		MOV			R2, #0
		LDR			R3, =4096
FOR_LOOP:
		LDR			R1, [R0]
		STR			R1, [R7], #4
		
		@CMP			R1, #0x07
		@LDREQ		R2, =0x50000000
		@CMPEQ		R7, R2
		@MOVEQ		PC, LR
		@B			WHILE_LOOP_END
		
		ADD			R9, R9, #4
		ADD			R2, R2, #4
		CMP			R9, R8
		BCS			WHILE_LOOP_END
		CMP			R2, R3
		BNE			FOR_LOOP
		
		@ 当读完一页的数据后，当前要读取的地址再加一页的长度继续读取
		ADD			R6, R6, R3
		B			WHILE_LOOP
		
WHILE_LOOP_END:
		
		@ 7. 取消选中NAND
		LDR			R0, =NFCONT
		LDR			R1, [R0]
		ORR			R1, #2
		STR			R1, [R0]
		
/*****************************************************************************/
		MOV			PC, LR
