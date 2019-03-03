
void delay(volatile int count)
{
	volatile int i = count;
	while(i)
	{
		i--;
	}
}

//int i = 0xf;  /* λ�����ݶ� */
int i = 0;    /* λ��BSS�� */
volatile const int j = 0x12345678;  /* λ��ֻ�����ݶ� */
//volatile int k=0;   /* bss�� */

int main()
{
	/* ����GPMCON��GPM0,1,2,3��Ϊ������� */
	volatile unsigned long *gpmcon = (volatile unsigned long *)0x7F008820;
	volatile unsigned long *gpmdat = (volatile unsigned long *)0x7F008824;

	*gpmcon &= ~(0xffff);
	*gpmcon |= 0x1111;
		
	/* ѭ��������4��LED */
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
