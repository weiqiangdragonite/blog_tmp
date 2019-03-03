/******************************************************************************
* OK6410 UART
* For GNU ASM
*
* uart.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-12
******************************************************************************/


		.global INIT_UART
		.global SEND_DATA
		
/*****************************************************************************/

INIT_UART:

		@ INIT_UART
		@ 1. 设置引脚GPA0和GPA1为使用串口模式
		LDR			R0, =0x7F008000
		LDR			R1, [R0]
		BIC			R1, #0xFF
		ORR			R1, #0x22
		STR			R1, [R0]
		
		@ 2. 通过UBRDIVn设置波特率为115200bps
		LDR			R0, =0x7F005028
		MOV			R1, #34
		STR			R1, [R0]
		
		LDR			R0, =0x7F00502C
		LDR			R1, =0xDFDD
		STR			R1, [R0]
		
		@ 3. 通过ULCONn设置传输格式
		LDR			R0, =0x7F005000
		MOV			R1, #3
		STR			R1, [R0]
		
		@ 4. 通过UCONn设置通道工作模式
		LDR			R0, =0x7F005004
		LDR			R1, [R0]
		LDR			R2, =0xFFFFF000
		AND			R1, R2
		LDR			R2, =0x805
		ORR			R1, R2
		STR			R1, [R0]
		
		@ 5. 设置UFCONn为禁止FIFO模式
		LDR			R0, =0x7F005008
		MOV			R1, #0
		STR			R1, [R0]
		
		@ 6. 设置UMCONn为禁止自动流控制
		LDR			R0, =0x7F00500C
		STR			R1, [R0]
		
INIT_UART_END:
		MOV			PC, LR
		
/*****************************************************************************/

SEND_DATA:

		@ 
		MOV			R12, LR
		
		@ R1 为参数，转为8个ASCII码
		@LDR			R1, =0xE3A00207
		
		@ R2暂存R1的值
		MOV			R2, R1
		
		
		@ 取0 ~ 3位
		AND			R2, #0xF
		@ 转换为第1个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取4 ~ 7位，并右移至最低位
		MOV			R2, R2, LSR #4
		AND			R2, #0xF
		@ 转换为第2个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取8 ~ 11位，并右移至最低位
		MOV			R2, R2, LSR #8
		AND			R2, #0xF
		@ 转换为第3个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取12 ~ 15位，并右移至最低位
		MOV			R2, R2, LSR #12
		AND			R2, #0xF
		@ 转换为第4个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取16 ~ 19位，并右移至最低位
		MOV			R2, R2, LSR #16
		AND			R2, #0xF
		@ 转换为第5个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取20 ~ 23位，并右移至最低位
		MOV			R2, R2, LSR #20
		AND			R2, #0xF
		@ 转换为第6个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取24 ~ 27位，并右移至最低位
		MOV			R2, R2, LSR #24
		AND			R2, #0xF
		@ 转换为第7个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		MOV			R2, R1
		@ 取28 ~ 31位，并右移至最低位
		MOV			R2, R2, LSR #28
		AND			R2, #0xF
		@ 转换为第8个ASCII码
		ADD			R2, R2, #0x30
		@ 发送到串口
		BL			SEND_TO_UART
		
		
		@ 发送一个空格
		MOV			R2, #0xA
		@ 发送到串口
		BL			SEND_TO_UART
		
		
SEND_DATA_END:
		MOV			LR, R12
		MOV			PC, LR
		
/*****************************************************************************/

SEND_TO_UART:

		LDR			R3, =0x7F005010
WAIT_EMPTY:
		LDR			R4, [R3]
		AND			R4, #6
		CMP			R4, #6
		BNE			WAIT_EMPTY
		
		LDR			R3, =0x7F005020
		STRB		R2, [R3]
		
		MOV			PC, LR

		
/*****************************************************************************/
