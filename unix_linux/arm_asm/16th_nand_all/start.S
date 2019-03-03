
.globl _start
_start:

/* 硬件相关的设置 */
    /* Peri port setup */
    ldr r0, =0x70000000
    orr r0, r0, #0x13
    mcr p15,0,r0,c15,c2,4       @ 256M(0x70000000-0x7fffffff)
    
/* 关看门狗 */
/* 往WTCON(0x7E004000)写0 */
	
	ldr r0, =0x7E004000
	mov r1, #0
	str r1, [r0]
	
	/* 设置栈 */
	ldr sp, =8*1024

	/* 设置时钟 */
	bl clock_init

	/* 设置DDR */
	bl ddr_init
	
	/* 重定位 */
/* 把程序的代码段、数据段复制到它的链接地址去 */	
	adr r0, _start   /* 获得_start指令当前所在的地址 : 0*/
	ldr r1, =_start  /* _start的链接地址 0x51000000 */
	
	ldr r2, =bss_start      /* bss段的起始链接地址 */
	
	sub r2, r2, r1
	
	cmp r0,r1
	beq clean_bss
	
	bl copy2ddr
	cmp r0, #0
	bne halt
		
	/* 清BSS */
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
	ldr pc, =main
halt:
	b halt	
