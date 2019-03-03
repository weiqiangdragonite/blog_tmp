void delay()
{
	volatile int i = 0x10000;
	while (i--);
}

int xxxxx()
{
	int i = 0;
	
	volatile unsigned long *gpmcon = (volatile unsigned long *)0x7F008820;
	volatile unsigned long *gpmdat = (volatile unsigned long *)0x7F008824;
	
	/* gpm0,1,2,3设为输出引脚 */
	*gpmcon = 0x1111;
	
	while (1)
	{
		*gpmdat = i;
		i++;
		if (i == 16)
			i = 0;
		delay();
	}
	
	return 0;
}

