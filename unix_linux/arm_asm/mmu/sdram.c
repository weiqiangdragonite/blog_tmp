/******************************************************************************
* File: sdram.c
* 
* Description:
* 	This function init the sdram.
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-13
******************************************************************************/


/* define the DRAM Controler Register address */
#define		P1MEMSTAT			(*(volatile unsigned int *) (0x7E001000))
#define		P1MEMCCMD			(*(volatile unsigned int *) (0x7E001004))
#define		P1DIRECTCMD			(*(volatile unsigned int *) (0x7E001008))
#define		P1MEMCFG			(*(volatile unsigned int *) (0x7E00100C))
#define		P1REFRESH			(*(volatile unsigned int *) (0x7E001010))
#define		P1CASLAT			(*(volatile unsigned int *) (0x7E001014))
#define		P1TDQSS				(*(volatile unsigned int *) (0x7E001018))
#define		P1TMRD				(*(volatile unsigned int *) (0x7E00101C))
#define		P1TRAS				(*(volatile unsigned int *) (0x7E001020))
#define		P1TRC				(*(volatile unsigned int *) (0x7E001024))
#define		P1TRCD				(*(volatile unsigned int *) (0x7E001028))
#define		P1TRFC				(*(volatile unsigned int *) (0x7E00102C))
#define		P1TRP				(*(volatile unsigned int *) (0x7E001030))
#define		P1TRRD				(*(volatile unsigned int *) (0x7E001034))
#define		P1TWR				(*(volatile unsigned int *) (0x7E001038))
#define		P1TWTR				(*(volatile unsigned int *) (0x7E00103C))
#define		P1TXP				(*(volatile unsigned int *) (0x7E001040))
#define		P1TXSR				(*(volatile unsigned int *) (0x7E001044))
#define		P1TESR				(*(volatile unsigned int *) (0x7E001048))
#define		P1MEMCFG2			(*(volatile unsigned int *) (0x7E00104C))
#define		P1CHIP0CFG			(*(volatile unsigned int *) (0x7E001200))
#define		MEMSYSCFG			(*(volatile unsigned int *) (0x7E00F120))


void initSdram(void)
{

	/** 设置DRAM控制器 */
	
	// 1. 设置P1MEMCCMD为Configure模式
	P1MEMCCMD = 4;
	
	// 2. 设置其它参数
	// 设置P1REFRESH刷新周期
	P1REFRESH = 1037;
	
	// 设置CAS Latency
	P1CASLAT = 6;
	
	// 设置DQSS
	P1TDQSS = 1;
	
	// 设置MRD
	P1TMRD = 2;
	
	// 设置RAS
	P1TRAS = 7;
	
	// 设置RC
	P1TRC = 10;
	
	// 设置RCD
	P1TRCD = (1 << 3) | 4;

	// 设置RFC
	P1TRFC = (8 << 5) | 11;
	
	// 设置RP
	P1TRP = (1 << 3) | 4;
	
	// 设置RRD
	P1TRRD = 3;
	
	// 设置WR
	P1TWR = 3;
	
	// 设置WTR
	P1TWTR = 2;
	
	// 设置XP
	P1TXP = 1;
	
	// 设置XSR
	P1TXSR = 17;
	
	// 设置ESR
	P1TESR = 17;
	
	// 3. 设置P1MEMCFG寄存器参数
	// 设低6位为011010，其它位不变
	P1MEMCFG = (P1MEMCFG & 0xFFFFFFC0) | 0x1A;
	
	// 4. 设置P1MEMCFG2
	// 设低13位为0_1011_0100_0101
	P1MEMCFG2 = 0xB45;
	
	// 5. 设置P1_chip_0_cfg
	// 设第16位为Bank-Row-Column organization
	P1CHIP0CFG |= (1 << 16);
    
	
	/** 设置MDDR的时序 */
	
	// 6. 设置MDDR的时序P1 DIRECTCMD
	// 设为NOP
	P1DIRECTCMD = 0xC0000;
	// 设为PrechargeAll
	P1DIRECTCMD = 0;
	// 设为Autorefresh
	P1DIRECTCMD = 0x40000;
	// 继续设为Autorefresh
	P1DIRECTCMD = 0x40000;
	// 设置MRS
	P1DIRECTCMD = 0x80032;
	// 设置EMRS
	P1DIRECTCMD = 0xA0000;
	
	// 7. 设置MEM_SYS_CFG
	MEMSYSCFG = 0;
	
	// 8. 设置P1MEMCCMD为Go模式
	P1MEMCCMD = 0;
	
	// 9. 等待P1MEMSTAT变为Ready
	while (1) {
		if ((P1MEMSTAT & 0x3) == 1) break;
	}

	return;
}

void copyToSdram(void)
{
    
    // 将第二部分代码复制到SDRAM中。因为第二部分代码的加载地址在连接时
    // 被指定为4096(4KB)之后，系统启动后，把代码复制到内存中0x5000_4000处，
    // 虚拟地址为0xB000_4000，复制的数据
    
    unsigned int *dest = (unsigned int *) 0x50004000;
    unsigned int *start = (unsigned int *) 4096;
    unsigned int *end = (unsigned int *) (8 * 1024);
    
    while (start < end) {
        *(dest++) = *(start++);
    }
    
    return;
}
