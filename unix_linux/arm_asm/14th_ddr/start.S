.globl _start
_start:
/* 硬件相关的设置 */
    /* Peri port setup */
    ldr r0, =0x70000000
    orr r0, r0, #0x13
    mcr p15,0,r0,c15,c2,4       @ 256M(0x70000000-0x7fffffff)
    
/* 关看门狗 */
/* 往WTCON(0x7E004000)写0 */
	
	ldr r0, =0x7E004000  /* 伪指令 */
	mov r1, #0
	str r1, [r0]

	bl clock_init
	
/* 为调用C函数准备环境 */
	ldr sp, =8*1024
	bl sdram_init

/* 重定位代码 */
/* 把程序的代码段、数据段复制到它的链接地址去 */	
	adr r0, _start   /* 获得_start指令当前所在的地址 : 0*/
	ldr r1, =_start  /* _start的链接地址 0x50000000 */
	
	ldr r2, =bss_start      /* bss段的起始链接地址 */
	
	cmp r0,r1
	beq clean_bss
	
copy_loop:
	ldr r3, [r0], #4
	str r3, [r1], #4
	cmp r1, r2
	bne copy_loop	


/* 把BSS段对应的内存清零 */
clean_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	mov r3, #0
	cmp r0, r1
	beq on_ddr
clean_loop:
	str r3, [r0], #4
	cmp r0, r1	
	bne clean_loop	

on_ddr:
/* 调用C函数 */
	ldr pc, =main  /* pc等于main的链接地址 */

