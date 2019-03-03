/*******************************************************************************
* U-Boot For OK6410(S3C6410)
* 256 MB Mobile DDR SDRAM - K4X1G163PC
* 4 GB NAND Flash - K9GAG08
*
* File: start.s
*
* By weiqiangdragonite@gmail.com
* Date start from: 2013-8-21
*******************************************************************************/


		.text
_start: .global _start


/*******************************************************************************
* Exception vector addresses
*******************************************************************************/

        @ 0x00000000  RESET (SVC) - reset
        b           reset
		@ 0x00000004  UND (UND) - undefined instruction
        ldr         pc, _undefined_instruction
		@ 0x00000008  SWI (SVC) - software interrupt
        ldr         pc, _software_interrupt
		@ 0x0000000C  PABT (ABT) - prefetch abort
		ldr         pc, _prefetch_abort
		@ 0x00000010  DABT (ABT) - data abort
		ldr         pc, _data_abort
		@ 0x00000014  reserved
		ldr         pc, _reserved
		@ 0x00000018  IRQ (IRQ) - interrupt request
		ldr         pc, _irq
		@ 0x0000001C  FIQ (FIQ) - fast interrupt request
		ldr			pc, _fiq
        
        ldr         pc, =test
		
_undefined_instruction:
        .word undefined_instruction
_software_interrupt:
        .word software_interrupt
_prefetch_abort:
        .word prefetch_abort
_data_abort:
        .word data_abort
_reserved:
        .word reserved
_irq:
        .word irq
_fiq:
        .word fiq


/******************************************************************************/

reset:

        nop
        nop
halt:
        b           halt







/*******************************************************************************
* Exceptoion handlers
*******************************************************************************


undefined_instruction:
        bl          do_undefined_instruction
        
software_interrupt:
        bl          do_software_interrupt
        
prefetch_abort:
        bl          do_prefetch_abort
        
data_abort:
        bl          do_data_abort
        
reserved:
        bl          do_reserved
        
irq:
        bl          do_irq
        
fiq:
        bl          do_fiq
        */

undefined_instruction:
        bl          halt
        
software_interrupt:
        bl          halt
        
prefetch_abort:
        bl          halt
        
data_abort:
        bl          halt
        
reserved:
        bl          halt
        
irq:
        bl          halt
        
fiq:
        bl          halt
        
test:
        bl          halt
