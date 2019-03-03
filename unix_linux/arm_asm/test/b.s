	.global task
	.global abc

abc:
	.word 0xFF

task:
	ldr r1, abc
	ldr r0, [r1]
	mov pc, lr

