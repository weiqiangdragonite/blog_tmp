
void delay(volatile int count)
{
	volatile int i = count;
	while(i)
	{
		i--;
	}
}

//int i = 0xf;  /* 位于数据段 */
int i = 0;    /* 位于BSS段 */
volatile const int j = 0x12345678;  /* 位于只读数据段 */
//volatile int k=0;   /* bss段 */

int main()
{
	/* 配置GPMCON让GPM0,1,2,3作为输出引脚 */
	volatile unsigned long *gpmcon = (volatile unsigned long *)0x7F008820;
	volatile unsigned long *gpmdat = (volatile unsigned long *)0x7F008824;

	*gpmcon &= ~(0xffff);
	*gpmcon |= 0x1111;
		
	/* 循环点亮这4个LED */
	while (1)
	{
		*gpmdat &= ~0xf;
		*gpmdat |= i;
		i++;
		delay(20000);
		if (i == 16)
			i = 0;
	}
	
	return 0;
}
