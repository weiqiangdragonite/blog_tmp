#include "framebuffer.h"

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
#define UINTP0      (*((volatile unsigned long *)0x7F005030))
 

#define UINTM0    (*((volatile unsigned long *)0x7F005038))


#define GPACON     (*((volatile unsigned long *)0x7F008000))

#define ENABLE_FIFO 1

static void delay(void)
{
	volatile int i = 10;
	while (i--);
}

void uart_tx_int_enable(void)
{
	UINTM0 &= ~(1<<2);	
}

void uart_tx_int_disable(void)
{
	UINTM0 |= (1<<2);	
}


void init_uart(void)
{
	GPACON &= ~0xff;
	GPACON |= 0x22;
	
	/* ULCON0 */
	ULCON0 = 0x3;  /* ����λ:8, �޽���, ֹͣλ: 1, 8n1 */
	UCON0  = 0x5 | (1<<9);  /* ʹ��UART���͡�����, tx interrupt request type = level */
#ifdef ENABLE_FIFO
	UFCON0 = 0x07 | (1<<6); /* FIFO enable, tx fifo trigger level = 16 bytes */	
#else
	UFCON0 = 0x00; /* FIFO disable */
#endif
	UMCON0 = 0;
	
	/* ������ */
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
    UINTM0 = 0xF;
}

unsigned char getc(void)
{
#ifdef ENABLE_FIFO
	while ((UFSTAT0 & (1<<6)) == 0 && (UFSTAT0 & 0x3f) == 0)delay();
#else	
	while ((UTRSTAT0 & (1<<0)) == 0);
#endif
	
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


#define TX_BUF_LEN   2048
static unsigned char txbuf[2047];
static unsigned int r_idx = 0;
static unsigned int w_idx = 0;

static int isFull(void)
{
	if ((w_idx + 1) % TX_BUF_LEN == r_idx)
		return 1;
	else
		return 0;
}

static int isEmpty(void)
{
	return (w_idx == r_idx);
}

static int putData(unsigned char data)
{
	if (isFull())
		return -1;
	else
	{
		txbuf[w_idx] = data;
		w_idx = (w_idx + 1) % TX_BUF_LEN;
		return 0;
	}
}

static int getData(unsigned char *pdata)
{
	if (isEmpty())
	{
		return -1;
	}
	else
	{
		*pdata = txbuf[r_idx];
		r_idx = (r_idx + 1) % TX_BUF_LEN;
		return 0;
	}
}

void putc(char c)
{
	putData(c);    /* �����ݷŵ���������ȥ */

	/* ���"uart �����ж�"δʹ�ܵĻ���ʹ��"uart �����ж�" */
	uart_tx_int_enable();

	lcd_putc(c);
}

void do_uart_irq(void)
{
	int i;
	int cnt;
	unsigned char c;
	
	if (UINTP0 & (1<<2))
	{
		/* ���ڷ����ж� */
		 if (isEmpty())
		 {
		 	/* ��ֹ�ж� */
			uart_tx_int_disable();
		 }
		 else
		 {
		 	/* �ӻ��ͻ�������ȡ������, �ŵ�TX FIFO��ȥ */
			cnt = (UFSTAT0 >> 8) & 0x3f;
			cnt = 64 - cnt;
			for (i = 0; i < cnt; i++)
			{
				if (getData(&c) == 0)
				{
					UTXH0 = c;
				}
				else
				{
					break;
				}
			}
		 }
	}

	else if (UINTP0 & (1<<0))
	{
		/* ���ڽ����ж�, ��RX FIFO��ȡ������ */
	}

	/* ���ж� */
	UINTP0 = 0xf;
}


