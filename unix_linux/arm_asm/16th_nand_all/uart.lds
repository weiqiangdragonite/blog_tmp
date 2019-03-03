SECTIONS {
    . = 0x51000000;
    
	.text : {
			start.o
			clock.o
			sdram.o
			nand.o
			* (.text)
	}

	.rodata : {
			* (.rodata)
	}

	.data : {
			* (.data)
	}

    bss_start = .;
    .bss ALIGN(4)  : { *(.bss)  *(COMMON) }
    bss_end = .;
}
