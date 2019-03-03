SECTIONS
{
    . = 0x50000000;         /* 连接地址从0x00000000开始 */
	
    .text :	                /* 所有代码段 */
    {
		init.o
        * (.text)
    }
    . = ALIGN(4);			/* 4字节对齐 */
	
    .rodata :               /* 神马来的 */
    {
        * (.rodata)
    }
    . = ALIGN(4);
	
    .data :					/* 数据段 */
    {
        * (.data)
    }
    . = ALIGN(4);
	
    bss_start = . ;			/* BSS段开始地址 */
    .bss :					/* BSS段 */
    {
        * (.bss)
		* (.common)
    }
    bss_end = . ;			/* BSS段结束地址 */
}