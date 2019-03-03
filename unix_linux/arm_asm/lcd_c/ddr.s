/*******************************************************************************
* OK6410 Mobile DDR SDRAM
* For GNU ASM
*
* ddr.s
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-9
*******************************************************************************/


		.global INIT_DDR
		
/******************************************************************************/

INIT_DDR:

		@ 设置DRAM控制器
		@ 1. 设置P1MEMCCMD为Configure模式
		LDR			R0, =0x7E001004
		MOV			R1, #0x4
		STR			R1, [R0]
		
		@ 2. 设置其它参数
		@ 设置P1REFRESH刷新周期
		LDR			R0, =0x7E001010
		LDR			R1, =1037
		STR			R1, [R0]
		
		@ 设置CAS Latency
		LDR			R0, =0x7E001014
		MOV			R1, #6
		STR			R1, [R0]
		
		@ 设置DQSS
		LDR			R0, =0x7E001018
		MOV			R1, #1
		STR			R1, [R0]
		
		@ 设置MRD
		LDR			R0, =0x7E00101C
		MOV			R1, #2
		STR			R1, [R0]
		
		@ 设置RAS
		LDR			R0, =0x7E001020
		MOV			R1, #7
		STR			R1, [R0]
		
		@ 设置RC
		LDR			R0, =0x7E001024
		MOV			R1, #10
		STR			R1, [R0]
		
		@ 设置RCD
		LDR			R0, =0x7E001028
		MOV			R1, #0xC
		STR			R1, [R0]
		
		@ 设置RFC
		LDR			R0, =0x7E00102C
		LDR			R1, =0x10B
		STR			R1, [R0]
		
		@ 设置RP
		LDR			R0, =0x7E001030
		MOV			R1, #0xC
		STR			R1, [R0]
		
		@ 设置RRD
		LDR			R0, =0x7E001034
		MOV			R1, #3
		STR			R1, [R0]
		
		@ 设置WR
		LDR			R0, =0x7E001038
		MOV			R1, #3
		STR			R1, [R0]
		
		@ 设置WTR
		LDR			R0, =0x7E00103C
		MOV			R1, #2
		STR			R1, [R0]
		
		@ 设置XP
		LDR			R0, =0x7E001040
		MOV			R1, #1
		STR			R1, [R0]
		
		@ 设置XSR
		LDR			R0, =0x7E001044
		MOV			R1, #17
		STR			R1, [R0]
		
		@ 设置ESR
		LDR			R0, =0x7E001048
		MOV			R1, #17
		STR			R1, [R0]
		
		@ 3. 设置P1MEMCFG
		LDR			R0, =0x7E00100C
		@ 设低6位为011010，其它位不变
		LDR			R1, [R0]
		BIC			R1, #0x3F
		ORR			R1, #0x1A
		STR			R1, [R0]
		
		@ 4. 设置P1MEMCFG2
		LDR			R0, =0x7E00104C
		@ 设低13位为0_1011_0100_0101
		LDR			R1, [R0]
		LDR			R2, =0xFFFFE000
		AND			R1, R2
		LDR			R2, =0x00000B45
		ORR			R1, R2
		STR			R1, [R0]
		
		@ 5. 设置P1_chip_0_cfg
		LDR			R0, =0x7E001200
		@ 设第16位为Bank-Row-Column organization
        LDR         R2, =0x00010000
        ORR			R1, R2
		STR			R1, [R0]
		
		@ 设置MDDR
		@ 6. 下面开始设置MDDR的时序P1 DIRECTCMD
		LDR			R0, =0x7E001008
		@ 设为NOP
		LDR			R1, =0xC0000
		STR			R1, [R0]
		@ 设为PrechargeAll
		MOV			R1, #0
		STR			R1, [R0]
		@ 设为Autorefresh
		LDR			R1, =0x40000
		STR			R1, [R0]
		@ 继续设为Autorefresh
		LDR			R1, =0x40000
		STR			R1, [R0]
		@ 设置MRS
		LDR			R1, =0x80032
		STR			R1, [R0]
		@ 设置EMRS
		LDR			R1, =0xA0000
		STR			R1, [R0]
		
		@ 7. 设置MEM_SYS_CFG
		LDR			R0, =0x7E00F120
		MOV			R1, #0
		STR			R1, [R0]
		
		@ 8. 设置P1MEMCCMD为Go模式
		LDR			R0, =0x7E001004
		MOV			R1, #0
		STR			R1, [R0]
		
		@ 9. 等待P1MEMSTAT变为Ready
		LDR			R0, =0x7E001000
DDR_LOOP:
		LDR			R1, [R0]
		AND			R1, #3
		CMP			R1, #1
		BNE			DDR_LOOP
		
/******************************************************************************/
		MOV			PC, LR

