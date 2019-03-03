SECTIONS
{

    first 0x00000000 : {
        start.o
        clock.o
        sdram.o
        mmu.o
    }
    
    second 0xB0004000 : AT(4096) {
        * (.text)
    }
    
    . = ALIGN(4);
    bss_start = . ;			/* BSS段开始地址 */
    .bss :					/* BSS段 */
    {
        * (.bss)
    }
    bss_end = . ;			/* BSS段结束地址 */
}
