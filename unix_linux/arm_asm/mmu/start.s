/*******************************************************************************
* Ok6410
*
* S3C6410的SDRAM的物理地址范围是0x5000_0000 ~ 0x5FFF_FFFF(256 MB),
* 寄存器的物理地址范围是0x7000_0000 ~ 0x7F00_FFFF
*
* 这里将开启MMU，并将虚拟地址空间
*
* 程序代码分成两部分:
* 第一部分的运行地址设为0，它用来初始化SDRAM、复制第二部分的代码到SDRAM中、
* 设置页表、启动MMU、最后跳到SDRAM中继续执行。
* 第二部分的代码用来点亮LED。
*
* 程序流程:
* 关闭看门狗 -> 设置栈指针 -> 初始化SDRAM -> 复制第二部分代码到SDRAM中 ->
* 设置页表 -> 启动MMU -> 重设栈指针 -> 跳到第二部分代码处 -> 循环点亮LED
*
* 本程序只使用一级页表，以段的方式进行地址映射。
* 一级页表中使用4096个描述符来表示这4GB空间，
* 每个描述符对应1MB的虚拟地址，每个描述符占用4字节，
* 所以一级页表占用16KB的空间。
* 本程序使用SDRAM的开始16KB来存放一级页表，所以剩下的内存
* 开始物理地址为0x5000_4000。
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-25
*
*******************************************************************************/


        .text
_start: .global _start


/*******************************************************************************
*
*******************************************************************************/


        /* peri port setup */
        ldr r0, =0x70000000
        orr r0, r0, #0x13
        mcr p15, 0, r0, c15, c2, 4
        
        /* disable watchdog */
        ldr r0, =0x7E004000
        mov r1, #0
        str r1, [r0]
        
        /* set the stack */
        ldr sp, =(8 * 1024)
        
        /* init the clock */
        bl initClock
        
        /* init the SDRAM */
        bl initSdram
        
        /* copy the code from nand to sdram */
        @ldr r0, =bss_start      @ bss_start's address
        @ldr r1, =bss_end
        bl copyToSdram
        
        /* init the mmu */
        /* PS: after enable MMU, all the address is virtual address */
        bl initMMU
        
        /* jump to the main code */
        ldr pc, =main
        
/******************************************************************************/
        .end
        