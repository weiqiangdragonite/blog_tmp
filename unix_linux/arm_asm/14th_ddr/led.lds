SECTIONS 
{
    . = 0x50000000;
    .text : 
    { 
        start.o 
        * (.text)
    } 
    
    . = ALIGN(4);
    .rodata :
    {
    	* (.rodata)
    }
    
    . = ALIGN(4);
	.data :
	{
		* (.data)
	}

    . = ALIGN(4);	
    
    bss_start = . ;  /* 0x50000450 */
    
	.bss :
	{
		* (.bss)     /* i */
		* (.common)
	}
	
	bss_end = . ;   /* 0x50000450 */
}
