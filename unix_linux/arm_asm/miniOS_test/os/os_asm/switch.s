

@ void switch_process(PCB *pcur, PCB *pnext)
@ pcur是当前进程PCB地址，保存在R0中
@ pnext是新进程PCB地址，保存在R1中


.include "common_asm.h"

.equ    CONTENT_OFFSET,     20


    .global switch_process
switch_process:


@ 保存上下文:
save_content:

    @ 取出当前cpsr的值存到r2
    mrs r2, cpsr
    @ 取出低5位
    and r2, r2, #0x1F
    @ 判断是否为irq模式
    cmp r2, #0x12
    @ 若是，设置irq模式下的SP栈指针
    ldreq sp, =IRQ_STACK_START
    @ 否则设置管理模式下的SP栈指针
    ldrne sp, =SVC_STACK_START
    
    
    @ R0指向当前进程PCB，偏移到PCB结构体content处，CONTENT_OFFSET=20
    add r0, r0, #CONTENT_OFFSET
    @ 取出spsr的值(spsr保存的是用户模式下的cpsr)
    mrs r2, spsr
    @ 保存到PCB结构体的content[0]
    stmia r0!, {r2}
    @ 保存挂起的用户空间程序SP、LR到content[1-2]
    @ 用于将来恢复用户SP、LR寄存器
    stmia r0!, {sp}^
    stmia r0!, {lr}^
    
    @ 把中断处理时保存的寄存器(R0-R12,LR)保存到PCB中
    @ 因为在中断时，我们先保存上下文，即保存R0-R12，R14到中断模式下
    @ 的SP指针处，所以要先把保存的内容取出来，此时SP指向的是栈开始处的地址，
    @ 因为保存时是从高地址到低地址保存的，寄存器也是从高到低排列，
    @ sp是先减后存，比如sp=0x5FFF5FF0，则5FFF5FEC处保存LR
    @ 所以先将sp移到R0处，即0x5FFF5FF0 - 14 * 4(0x38) = 0x5FFF5FB0
    sub sp, sp, #0x38
    @ 取出R0-R10的值保存到寄存器R2-R12中， {r2-r12} <- [sp]
    ldmia sp!, {r2-r12}
    @ 然后再保存到PCB中content[3-12]，[r0] <- {r2-r12}
    stmia r0!, {r2-r12}
    @ 然后再取剩下的R11、R12和LR
    ldmia sp!, {r2-r4}
    stmia r0!, {r2-r4}
    
    
    
    
    @ 清除PWM中断(IRQ)
    @ SWI中断不用清
    mrs r2, cpsr
    and r2, r2, #0x1F
    @ 判断是否为irq模式
    cmp r2, #0x12
    bne restore_content
    
    ldr r2, =0x7F006044     @ TINT_CSTAT
    ldr r3, [r2]
    orr r3, #0x20
    str r3, [r2]
    
    ldr r2, =0x71200F00     @ VIC0ADDRESS
    mov r3, #0
    str r3, [r2]
    
    
    
    
@ 恢复上下文:
restore_content:

    @ r1指向新进程PCB地址
    add r1, r1, #CONTENT_OFFSET
    @ 从content[0-2]中取出用户模式下的CPSR、SP和LR，加载到R2-R4
    ldmia r1!, {r2-r4}
    @ 先将cpsr保存到spsr中，将来一并恢复
    msr spsr_cxsf, r2
    @ 保存当前模式到R2中
    mrs r2, cpsr
    @ 切换到系统模式，设置用户模式下的SP和LR
    msr cpsr_c, #0xdf
    mov sp, r3
    mov lr, r4
    @ 切换回当前模式
    msr cpsr_cxsf, r2
    @ 将PCB保存数据恢复到R0-R12中，LR保存到PC中，返回用户程序
    ldmia r1, {r0-r12, pc}^