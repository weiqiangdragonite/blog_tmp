
#define ULCON0     (*((volatile unsigned long *)0x7F005000))
#define UCON0      (*((volatile unsigned long *)0x7F005004))
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define UMCON0     (*((volatile unsigned long *)0x7F00500C))
#define UTRSTAT0   (*((volatile unsigned long *)0x7F005010))
#define UFSTAT0    (*((volatile unsigned long *)0x7F005018))
#define UTXH0      (*((volatile unsigned char *)0x7F005020))
#define URXH0      (*((volatile unsigned char *)0x7F005024))
#define UBRDIV0    (*((volatile unsigned short *)0x7F005028))
#define UDIVSLOT0  (*((volatile unsigned short *)0x7F00502C))

#define GPACON     (*((volatile unsigned long *)0x7F008000))

#define ENABLE_FIFO 

void init_uart(void)
{
	GPACON &= ~0xff;
	GPACON |= 0x22;
	
	/* ULCON0 */
	ULCON0 = 0x3;  /* 数据位:8, 无较验, 停止位: 1, 8n1 */
	UCON0  = 0x5;  /* 使能UART发送、接收 */
	UFCON0 = 0x01; /* FIFO ENABLE */

	UMCON0 = 0;
	
	/* 波特率 */
	/* DIV_VAL = (PCLK / (bps x 16 ) ) - 1 
	 * bps = 57600
	 * DIV_VAL = (66500000 / (115200 x 16 ) ) - 1 
	 *         = 35.08
	 */
	UBRDIV0   = 35;

	/* x/16 = 0.08
	 * x = 1
	 */
	UDIVSLOT0 = 0x1;
	
}

unsigned char getc(void)
{
	while ((UFSTAT0 & (1<<6)) == 0 && (UFSTAT0 & 0x3f) == 0);
	return URXH0;
}

int getc_nowait(unsigned char *pChar)
{
#ifdef ENABLE_FIFO
	if ((UFSTAT0 & (1<<6)) == 0 && (UFSTAT0 & 0x3f) == 0)
#else
	if ((UTRSTAT0 & (1<<0)) == 0)
#endif		
	{
		return -1;
	}
	else
	{	
		*pChar = URXH0;
		return 0;
	}
}

void putc(char c)
{
	while (UFSTAT0 & (1<<14));
	UTXH0 = c;
}


