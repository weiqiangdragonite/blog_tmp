SECTIONS
{
    . = 0x00001C00;         /* 连接地址从0x00000000开始 */
    . = ALIGN(4);			/* 4字节对齐 */
    .text :	                /* 所有代码段 */
    {
        * (.text)
    }
    
    . = ALIGN(4);
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
    }
    bss_end = . ;			/* BSS段结束地址 */
}

