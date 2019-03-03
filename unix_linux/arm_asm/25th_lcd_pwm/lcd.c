
#include <stdio.h>
#include "framebuffer.h"
#include "pwm.h"

#define GPECON  (*((volatile unsigned long *)0x7F008080))
#define GPEDAT  (*((volatile unsigned long *)0x7F008084))
#define GPFCON  (*((volatile unsigned long *)0x7F0080A0))
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))
#define GPICON  (*((volatile unsigned long *)0x7F008100))
#define GPJCON  (*((volatile unsigned long *)0x7F008120))

/* display controller */
#define MIFPCON     (*((volatile unsigned long *)0x7410800C))
#define SPCON         (*((volatile unsigned long *)0x7F0081A0))
#define VIDCON0      (*((volatile unsigned long *)0x77100000))
#define VIDCON1      (*((volatile unsigned long *)0x77100004))
#define VIDTCON0     (*((volatile unsigned long *)0x77100010))
#define VIDTCON1     (*((volatile unsigned long *)0x77100014))
#define VIDTCON2     (*((volatile unsigned long *)0x77100018))
#define WINCON0      (*((volatile unsigned long *)0x77100020))
#define VIDOSD0A      (*((volatile unsigned long *)0x77100040))
#define VIDOSD0B      (*((volatile unsigned long *)0x77100044))
#define VIDOSD0C      (*((volatile unsigned long *)0x77100048))
#define VIDW00ADD0B0      (*((volatile unsigned long *)0x771000A0))
#define VIDW00ADD1B0      (*((volatile unsigned long *)0x771000D0))
#define VIDW00ADD2      (*((volatile unsigned long *)0x77100100))


#define  VSPW         9
#define  VBPD          1
#define  LINEVAL     271
#define  VFPD          1


#define  HSPW         40    
#define  HBPD          1
#define  HOZVAL      479
#define  HFPD          1

#define LeftTopX     0
#define LeftTopY     0

#define RightBotX   479
#define RightBotY   271

#define FRAME_BUFFER   0x54000000


unsigned int fb_base_addr;
unsigned int bpp;
unsigned int xsize;
unsigned int ysize;


void lcd_init(void)
{
	/* 1. 设置相关GPIO引脚用于LCD */
	GPICON = 0xaaaaaaaa;  /* gpi0~gpi15用作lcd_vd[0~15] */
	GPJCON = 0xaaaaaaa;   /* gpj0~gpi11用作lcd */
	GPFCON &= ~(0x3<<28);
	GPFCON |=  (1<<28);    /* GPF14用作背光使能信号 */
	GPECON &= ~(0xf);
	GPECON |= (0x1);          /* GPE0用作LCD的on/off信号 */
	
	/* 2. 初始化6410的display controller 
	 * 2.1 hsync,vsync,vclk,vden的极性和时间参数
	 * 2.2 行数、列数(分辨率),象素颜色的格式
	 * 2.3 分配显存(frame buffer),写入display controller
	 */

	MIFPCON &= ~(1<<3);   /* Normal mode */

	SPCON    &= ~(0x3);
	SPCON    |= 0x1;            /* RGB I/F style */

#if 0
	VIDCON0 &= ~((3<<26) | (3<<17) | (0xff<<6));     /* RGB I/F, RGB Parallel format,  */
	VIDCON0 |= ((2<<6) | (1<<4) | (0x3<<2));      /* vclk== 27MHz Ext Clock input / (CLKVAL+1) = 27/3 = 9MHz */
#else
	VIDCON0 &= ~((3<<26) | (3<<17) | (0xff<<6)  | (3<<2));     /* RGB I/F, RGB Parallel format,  */
	VIDCON0 |= ((14<<6) | (1<<4) );      /* vclk== HCLK / (CLKVAL+1) = 133/15 = 9MHz */
#endif

	VIDCON1 &= ~(1<<7);   /* 在vclk的下降沿获取数据 */
	VIDCON1 |= ((1<<6) | (1<<5));  /* HSYNC高电平有效, VSYNC高电平有效, */

	VIDTCON0 = (VBPD << 16) | (VFPD << 8) | (VSPW << 0);
	VIDTCON1 = (HBPD << 16) | (HFPD << 8) | (HSPW << 0);
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0);

	WINCON0 &= ~(0xf << 2);
	WINCON0 |= (0xb<<2);    /* unpacked 24 BPP (non-palletized R:8-G:8-B:8 ) */

	VIDOSD0A = (LeftTopX<<11) | (LeftTopY << 0);
	VIDOSD0B = (RightBotX<<11) | (RightBotY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1);  /* 272 * 480 */

	VIDW00ADD0B0 = FRAME_BUFFER;
	VIDW00ADD1B0 =  (((HOZVAL + 1)*4 + 0) * (LINEVAL + 1)) & (0xffffff);
	                                /* VBASEL = VBASEU + (LINEWIDTH+OFFSIZE) x (LINEVAL+1) 
                          *        = 0 + (480*4 + 0) * 272
                          *        = 
	                     */
	//VIDW00ADD2 =  HOZVAL + 1;
	fb_base_addr = FRAME_BUFFER;
	xsize = HOZVAL + 1;
	ysize = LINEVAL + 1;
	bpp   = 24;
}


void backlight_enable(void)
{
	GPFDAT |= (1<<14);
}

void backlight_disable(void)
{
	GPFDAT &= ~(1<<14);
}


void lcd_on(void)
{
	GPEDAT |= (1<<0);
	/* 等待10 frame */
}

void lcd_off(void)
{
	GPEDAT &= ~(1<<0);
}

void displaycon_on(void)
{
	VIDCON0 |= 0x3;
	WINCON0 |= 1;
}

void displaycon_off(void)
{
	VIDCON0 &= ~0x3;
	WINCON0 &= ~1;
}

void lcd_enable(void)
{
	/* 打开背光 */
	/* 使能LCD本身 */
	/* 使能display controller */
}

void lcd_disable(void)
{
	/* 关闭背光 */
	/* 关闭LCD本身 */
	/* 关闭display controller */
}

void draw_line(void)
{
	DrawLine(0,0, 0,271, 0xff0000);
	DrawLine(0,0, 479,0, 0x00ff00);
	DrawLine(0,0, 479,271, 0x0000ff);
	DrawLine(0,271, 479,0, 0x00ffff);
	DrawLine(0,271, 479,271, 0xff00ff);
	DrawLine(479,271, 479,0, 0xffff00);
	DrawLine(0,136, 479,136, 0x0123ff);
	DrawLine(240,0, 240,271, 0x0567ff);
}

void display_red(void)
{
	volatile unsigned long *p = (volatile unsigned long *)FRAME_BUFFER;
	int x, y;
	int cnt = 0;
	unsigned long colors[] = {0xff0000, 0x00ff00, 0x0000ff, 0, 0xffffff};
	static int color_idx = 0;
	
	for (y = 0; y <= LINEVAL; y++)
	{
		for (x = 0; x <= HOZVAL; x++)
		{
			p[cnt++] =colors[color_idx] ;  /* red */
		}
	}

	color_idx++;
	if (color_idx == 5)
		color_idx = 0;
}

void lcd_test(void)
{
	unsigned char c;
	static int lcdon = 0;
	static int blon = 0;
	static int dispon = 0;

	lcd_init();
	
	while (1)
	{
		printf("********LCD TEST MENU********\n\r");
		printf("[L] enable/disable LCD\n\r");
		printf("[B] enable/disable back light\n\r");
		printf("[C] enable/disable s3c6410 display controller\n\r");
		printf("[D] display color\n\r");
		printf("[I]  draw line\n\r");
		printf("[Q] quit\n\r");

		do {
			c = getc();
			if (c == '\n' || c == '\r')
			{
				printf("\n\r");
			}
			else
			{
				putc(c);
			}
		} while (c == '\n' || c == '\r');

		switch (c) {
			case 'l':
			case 'L':
			{
				if (lcdon)
				{
					lcd_off();
					printf("LCD off\n\r");
				}
				else
				{
					lcd_on();
					printf("LCD on\n\r");
				}
				lcdon = !lcdon;
				break;
			}


			case 'b':
			case 'B':
			{
				pwm_menu();
				break;
			}


			case 'c':
			case 'C':
			{
				if (dispon)
				{
					displaycon_off();
					printf("Display controller off\n\r");
				}
				else
				{
					displaycon_on();
					printf("Display controller on\n\r");
				}
				blon = !blon;
				break;
			}

			case 'd':
			case 'D':
			{
				display_red();
				break;
			}

			case 'i':
			case 'I':
			{
				draw_line();
				break;
			}

			case 'q':
			case 'Q':
			{
				return ;
				break;
			}

		}

	}
}

