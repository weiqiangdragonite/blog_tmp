/******************************************************************************
* OK6410 Buzzer
*
* By weiqiangdragonite@gmail.com
*
* Date: 2013-8-1
*
* Use for GNU - ARM
*
******************************************************************************/

		.text									@ 以下为代码段
_start:	.global _start							@ '_start'是链接器所必须的
		@.global init							@ 'init'是初始化程序
		@.global main							@ 'main'是主程序
		@.global beep							@ 'beep'是鸣叫设置
		@.global delay							@ 'delay'是延时程序
		@.global delay_start
		@.global delay_end
		
		bl init									@ 跳转到初始化程序
		bl main									@ 跳转到主程序
		bl beep
	
/*	
main:		

	ldr r1, =0x7F008820
	mov r0, #0x1000
	str r0, [r1]


	@ldr r1, =0x7F008824
	@mov r0, #0
	@str r0, [r1]
	
	mov pc, lr
	
beep:
	
	ldr r1, =0x7F008824
	mov r0, #0
	str r0, [r1]
	
	bl delay
	
	ldr r1, =0x7F008824
	mov r0, #0xf
	str r0, [r1]
	
	bl delay
	
	b beep
*/
/*****************************************************************************/

init:
		@ 硬件相关的设置：把外设的基地址告诉CPU
		@ Peripheral port setup
		@ 外设地址是从0x7000_0000 到0x7F00_FFFF
		@ 外设地址的映射由协处理器设置
		@ 31 ~ 12位是基地址(base address)，0~4是大小(size)
		@ 基地址 0x7000_0000
		ldr r0, =0x70000000
		@ 大小是0x13，即10011，即256MB（因为ok6410是256M内存）
		orr r0, r0, #0x00000013
		@ 协处理指令（查看ARM1176jzfs.pdf）
		@ mcr{cond} p15, op1, rd, crn, crm, op2
		mcr p15, 0, r0, c15, c2, 4				@ 将ARM处理器的寄存器中的数据写到CP15中的寄存器中

		@ 把外设的地址告诉了CPU后，以后访问7开头的地址就是访问寄存器了
		@ 设置了以后才能访问寄存器
		
		
		@看门狗暂时还不懂
		@ 关看门狗(watch dog)
		@ 往WTCON(0x7E004000)最低位写0
		ldr r0, =0x7e004000
		@ 把r0指向的地址的数据加载到r1
		ldr r1, [r0]
		@ 最低位写0
		ldr r2, =0xfffffffe
		and r1, r2
		@ 把r1的数据加载到r0指向的内存的地址
		str r1, [r0]
		
		@ 返回
		mov pc, lr
		
		

		
/*****************************************************************************/

main:
		@ 设置GPFCON为输出模式
		ldr r0, =0x7f0080a0
		@ 取出数据
		ldr r1, [r0]
		@ 最高2位设为01
		ldr r2, =0x3fffffff
		and r1, r2
		ldr r2, =0x40000000
		orr r1, r2
		@ 存储数据
		str r1, [r0]
		
		@ 设置GPFPUD
		ldr r0, =0x7f0080a8
		@ 取出数据
		ldr r1, [r0]
		@ 最高2位设为00
		ldr r2, =0x3fffffff
		and r1, r2
		@ 存储数据
		str r1, [r0]
		
		
		
		@ 返回
		mov pc, lr
		
/*****************************************************************************/

beep:

		@ 设置GPFDAT最高位为高电平1，鸣叫
		ldr r0, =0x7f0080a4
		@ 取出数据
		ldr r1, [r0]
		
		bl delay
		bl delay
		bl delay
		bl delay
		
		@ GPFDAT最高位为高电平1，鸣叫
		ldr r2, =0xffff7fff
		and r1, r2
		ldr r2, =0x00008fff
		orr r1, r2
		@ 加载数据
		str r1, [r0]
		
		
		@ 延时
		bl delay
		
		@ GPFDAT最高位为低电平0，停止鸣叫
		ldr r2, =0xffff7fff
		and r1, r2
		str r1, [r0]
		
		@ 延时
		bl delay
		
		b  beep

		
/*****************************************************************************/

delay:
		mov r3, #0
delay_start:
		cmp r3, #0x00040000
		beq delay_end
		add r3, r3, #1
		b   delay_start
delay_end:
		mov pc, lr
		
/*****************************************************************************/
		
		nop
		nop
		.end
