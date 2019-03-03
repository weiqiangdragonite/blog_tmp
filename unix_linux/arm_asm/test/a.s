
.section .data
	.equ	ARM_SYS, 0x1
	.equ	ARM_ABT, 0x2


.section .text

.global _start
_start:
	mov r0, #ARM_SYS
	str r0, [r1]

	
	bl main

.end
