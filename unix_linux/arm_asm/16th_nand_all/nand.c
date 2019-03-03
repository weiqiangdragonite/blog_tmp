
#define MEM_SYS_CFG     (*((volatile unsigned long *)0x7E00F120))
#define NFCONF          (*((volatile unsigned long *)0x70200000))
#define NFCONT          (*((volatile unsigned long *)0x70200004))
#define NFCMMD          (*((volatile unsigned long *)0x70200008))
#define NFADDR          (*((volatile unsigned long *)0x7020000C))
#define NFDATA          (*((volatile unsigned char *)0x70200010))
#define NFSTAT          (*((volatile unsigned long *)0x70200028))

#define NAND_DISABLE_CE()	(NFCONT |= (1 << 1))
#define NAND_ENABLE_CE()	(NFCONT &= ~(1 << 1))
#define NF_TRANSRnB()		do { while(!(NFSTAT & (1 << 0))); } while(0)

#define NAND_CMD_RESET		0xff
#define NAND_CMD_READID		0x90
#define NAND_CMD_READ0		0
#define NAND_CMD_READSTART	0x30

void nand_select(void)
{
	NFCONT &= ~(1<<1);
}

void nand_deselect(void)
{
	NFCONT |= (1<<1);
}


void nand_cmd(unsigned char cmd)
{
	NFCMMD = cmd;
}

void nand_addr(unsigned char addr)
{
	NFADDR = addr;
}

unsigned char nand_get_data(void)
{
	return NFDATA;
}

void nand_send_data(unsigned char data)
{
	NFDATA = data;
}

void wait_ready(void)
{
	while ((NFSTAT & 0x1) == 0);
}

void nand_reset(void)
{
	/* ѡ�� */
	nand_select();
	
	/* ����0xff���� */
	nand_cmd(0xff);

	/* �ȴ����� */
	wait_ready();
	
	/* ȡ��ѡ�� */
	nand_deselect();
}


void nand_init(void)
{
	/* ��xm0csn2����nand flash cs0 Ƭѡ���� */
	MEM_SYS_CFG &= ~(1<<1);

	/* ����ʱ�����:hclk = 7.5ns */
#define TACLS     0
#define TWRPH0    3
#define TWRPH1    1
	NFCONF &= ~((1<<30) | (7<<12) | (7<<8) | (7<<4));
	NFCONF |= ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4));

	/* ʹ��nand flash controller */
	NFCONT |= 1;

	nand_reset();
}


void nand_send_addr(unsigned int addr)
{
#if 0	
	unsigned int page   = addr / 2048;
	unsigned int colunm = addr & (2048 - 1);

	/* ��������ַ��ʾ��ҳ�����￪ʼ */
	nand_addr(colunm & 0xff);
	nand_addr((colunm >> 8) & 0xff);

	/* ����������ַ��ʾ��һҳ */
	nand_addr(page & 0xff);
	nand_addr((page >> 8) & 0xff);
	nand_addr((page >> 16) & 0xff);
#else
	nand_addr(addr & 0xff);         /* a0~a7 */
	nand_addr((addr >> 8) & 0x1f);   /* ����ĽǶ�: a8~a12 */

	nand_addr((addr >> 13) & 0xff); /* ����ĽǶ�: a13~a20 */
	nand_addr((addr >> 21) & 0xff); /* ����ĽǶ�: a21~a28 */
	nand_addr((addr >> 29) & 0xff); /* ����ĽǶ�: a29~ */
	
#endif
}


int nand_read(unsigned int nand_start, unsigned int ddr_start, unsigned int len)
{
	unsigned int addr = nand_start;
	int i, count = 0;
	unsigned char *dest = (unsigned char *)ddr_start;
	
	/* ѡ��оƬ */
	nand_select();

	while (count < len)
	{
		/* ��������0x00 */
		nand_cmd(0x00);

		/* ������ַ */
		nand_send_addr(addr);

		/* ��������0x30 */
		nand_cmd(0x30);

		/* �ȴ����� */
		wait_ready();

		/* ������ */
		for (i = 0; i < 4096 && count < len; i++)
		{
			dest[count++] = nand_get_data();
		}

		addr += 4096;			
	}

	/* ȡ��Ƭѡ */
	nand_deselect();
	
	return 0;
}

static int nandll_read_page (unsigned char *buf, unsigned long addr, int large_block)
{
	int i;
	int page_size = 512;

	if (large_block==2)
	    page_size = 4096;

	NAND_ENABLE_CE();  // ѡ��nand

	NFCMMD = NAND_CMD_READ0;

	/* Write Address */
	NFADDR = 0;

	if (large_block)
	    NFADDR = 0;

	NFADDR = (addr) & 0xff;
	NFADDR = (addr >> 8) & 0xff;
	NFADDR = (addr >> 16) & 0xff;

	if (large_block)
	    NFCMMD = NAND_CMD_READSTART;

	NF_TRANSRnB();

	for(i=0; i < page_size; i++) {
	    *buf++ = NFDATA;
	}

	NAND_DISABLE_CE();

	return 0;
}

void nand_erase_block(unsigned long addr)
{
	int page = addr / 4096;
	
	nand_select();
	nand_cmd(0x60);
	
	nand_addr(page & 0xff);
	nand_addr((page >> 8) & 0xff);
	nand_addr((page >> 16) & 0xff);

	nand_cmd(0xd0);
	wait_ready();

	nand_deselect();
}

int nand_write(unsigned int nand_start, unsigned char * buf, unsigned int len)
{
	unsigned long count = 0;
	unsigned long addr  = nand_start;
	int i = nand_start % 4096;
	
	nand_select();
	while (count < len)
	{
		nand_cmd(0x80);
		nand_send_addr(addr);
		for (; i < 4096 && count < len; i++)
		{
			nand_send_data(buf[count++]);
			addr++;
		}

		nand_cmd(0x10);
		wait_ready();
		i = 0;		
	}

	nand_deselect();

	return 0;
}

int copy2ddr(unsigned int nand_start, unsigned int ddr_start, unsigned int len)
{
	unsigned char *dest = (unsigned char *)ddr_start;
	unsigned int free_page = 0;
	int i;

	if(len > 8192)
	{
		free_page = ((len - 8192)+4096) >> 12;
	}
	/* ��ʼ��nand flash controller */
	nand_init();
	
	/* ��nand flash */
	/* Read pages */
	for (i = 0; i < 4; i++, dest+=2048){
		nandll_read_page(dest, i, 2);
	}

	/* Read pages */
	for (i = 4; i < (free_page+4); i++, dest+=8192) {
		nandll_read_page(dest, i, 2);
	}
	
	return 0;
}

