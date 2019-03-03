/*******************************************************************************
* Project: miniOS
*
* File: start.s
*
* Description:
*

SDRAM: 256M
                        PA:
    +----------------+ 0x5FFFFFFF
 2M |   stack        |
    +----------------+ 0x5FE00000
 2M |   heap         |
    +----------------+ 0x5FC00000
 2M |   ...          |
    +----------------+ 0x5FA00000
    |                |
    |   process      | 240M
    |                |
    +----------------+ 0x50A00000
 1M |  page table    |
    +----------------+ 0x50900000
 1M |  frame buffer  |
    +----------------+ 0x50800000
 7M |   ...          |
    +----------------+ 0x50100000
 1M |   code         |
    +----------------+ 0x50000000

*
*******************************************************************************/

		.text
_start: .global _start

.include "common_asm.h"

/*******************************************************************************
* Exception vector addresses
*******************************************************************************/

        /* 0x00000000  RESET (SVC) - reset */
        b reset
		/* 0x00000004  UND (UND) - undefined instruction */
        ldr pc, =undefined_instruction
		/* 0x00000008  SWI (SVC) - software interrupt */
        ldr pc, =software_interrupt
		/* 0x0000000C  PABT (ABT) - prefetch abort */
		ldr pc, =prefetch_abort
		/* 0x00000010  DABT (ABT) - data abort */
		ldr pc, =data_abort
		/* 0x00000014  reserved */
		nop
		/* 0x00000018  IRQ (IRQ) - interrupt request */
		ldr pc, =irq
		/* 0x0000001C  FIQ (FIQ) - fast interrupt request */
		ldr pc, =fiq
        
/*******************************************************************************
* Reset
*******************************************************************************/

reset:
        /* use SVC mode, and turn off IQR & FIQ */
        msr cpsr_c, #0xD3
        
        /* peripheral port setup */
        ldr r0, =0x70000000
        orr r0, r0, #0x13
        mcr p15, 0, r0, c15, c2, 4
        
        /* disable watchdog */
        ldr r0, =0x7E004000
        mov r1, #0
        str r1, [r0]
        
        /* set the SVC stack */
        ldr sp, =(7 * 1024)
        
        /* init the clock */
        bl init_clock
        
        /* init the SDRAM */
        bl init_sdram
        
        @bl copy_to_sdram
        
        /* init the NAND Flash */
        bl init_nand
        
        
        /* relocate the code */
        adr r0, _start              @ the relative address of _start
        ldr r1, =_start             @ get the absolute address of _start
        ldr r2, =bss_start          @ get the bss_start address
        sub r2, r2, r1              @ get the copy code length (text and data)
        
		cmp r0, r1                  @ compare the relative address and
                                    @ absolute address
		beq clean_bss               @ if equal, go to clean bss
        
        /* copy the code from nand to sdram, parameter is r0, r1, r2 */
        bl copy_nand_to_sdram
        
        /* clean bss */
clean_bss:
        ldr r0, =bss_start
        ldr r1, =bss_end
        mov r2, #0
clean_loop:
        cmp r0, r1                  @ if bss_start != bss_end, clean
        strne r2, [r0], #4
        bne clean_loop
        
        
        /* reset all the stack 
.equ    STACK_START, 0x5FFFFFF0
.equ    STACK_SIZE, 32 * 1024

        msr cpsr_c, #0xD3
        ldr r0, =STACK_START        @ SVC stack, start form 0x5FFFFFF0
        mov sp, r0
        sub r0, r0, #STACK_SIZE     @ size 32 KB (0x8000)
        
        msr cpsr_c, #0xD7           @ ABT stack, start from 0x5FFF7FF0
        mov sp, r0
        sub r0, r0, #0x1000         @ size 4 KB (0x1000)
        
        msr cpsr_c, #0xD8           @ UND stack, start from 0x5FFF6FF0
        mov sp, r0
        sub r0, r0, #0x1000         @ size 4 KB (0x1000)
        
        msr cpsr_c, #0xD2           @ IRQ stack, start from 0x5FFF5FF0
        mov sp, r0
        sub r0, r0, #STACK_SIZE     @ size 32 KB (0x8000)
        
        msr cpsr_c, #0xD1           @ FIQ stack, start from 0x5FFEDFF0
        mov sp, r0
        sub r0, r0, #0x4000         @ size 16 KB (0x4000)
        
        msr cpsr_c, #0xDF           @ SYS stack, start from 0x5FFE9FF0
        mov sp, r0
        */

        msr cpsr_c, #0xD3           @ SVC stack
        ldr r0, =SVC_STACK_START
        mov sp, r0
        
        msr cpsr_c, #0xD7           @ ABT stack
        ldr r0, =ABT_STACK_START
        mov sp, r0
        
        msr cpsr_c, #0xD8           @ UND stack
        ldr r0, =UND_STACK_START
        mov sp, r0
        
        msr cpsr_c, #0xD2           @ IRQ stack
        ldr r0, =IRQ_STACK_START
        mov sp, r0
        
        msr cpsr_c, #0xD1           @ FIQ stack
        ldr r0, =FIQ_STACK_START
        mov sp, r0
        
        msr cpsr_c, #0xDF           @ SYS stack
        ldr r0, =SYS_STACK_START
        mov sp, r0
        
        
        
        
        /* set to SYS mode, and enable FIQ & IRQ */
        @msr cpsr_c, #0x1F
        
        /* set the return address */
        ldr lr, =halt
        
        /* jump to SDRAM, and run the main function */
        ldr pc, =main
        @bl main
        
halt:
        b halt
        
        
/*******************************************************************************
* Exception handlers
*******************************************************************************/

/* undefined instruction handler */
undefined_instruction:

        /* save the content */
        stmfd sp!, {r0-r12, lr}
        
        /* jump to handle function */
        bl handle_und
        
        /* restore the content */
        ldmfd sp!, {r0-r12, pc}^
        
        
/* software interrupt handler */
software_interrupt:

.equ    MAX_SWI_NUM,    6
        
        /* save the content */
        stmfd sp!, {r0-r12, lr}
        
        /* get the swi number */
        @ r0 ~ r3 store the caller para
        ldr r4, [lr, #-4]           @ swi address: lr - 4
        bic r4, #0xFF000000         @ the low 24 bits is swi number
        mov r1, r4
        @bl show_content
        @ldr r, =MAX_SWI_NUM
        @cmp r, r
        
        /* jump to handle function */
        @bl handle_swi
@.extern swi_table
        ldr r5, =swi_table
        ldr lr, =swi_end
        ldr pc, [r5, r4, lsl #2]
        
swi_end:
        /* restore the content */
        ldmfd sp!, {r0-r12, pc}^


/* prefetch abort handler */  
prefetch_abort:

        /* modify the return address */
        sub lr, lr, #4
        
        /* save the content */
        stmfd sp!, {r0-r12, lr}
        
        /* jump to handle function */
        bl handle_pabt
        
        /* restore the content */
        ldmfd sp!, {r0-r12, pc}^
        
        
/* data abort handler*/
data_abort:

        /* modify the return address */
        sub lr, lr, #8
        
        /* save the content */
        stmfd sp!, {r0-r12, lr}
        
        /* jump to handle function */
        bl handle_dabt
        
        /* restore the content */
        ldmfd sp!, {r0-r12, pc}^
        
        
/* irq handler */
irq:

        /* modify the return address */
        sub lr, lr, #4
        
        /* save the content */
        stmfd sp!, {r0-r12, lr}
        
        /* jump to handle function */
        bl handle_irq
        
        /* restore the content */
        ldmfd sp!, {r0-r12, pc}^
        
        
/* fiq handler */
fiq:

        /* modify the return address */
        sub lr, lr, #4
        
        /* save the content */
        stmfd sp!, {r0-r12, lr}
        
        /* jump to handle function */
        bl handle_fiq
        
        /* restore the content */
        ldmfd sp!, {r0-r12, pc}^

/*******************************************************************************
* END
*******************************************************************************/
        .end
        