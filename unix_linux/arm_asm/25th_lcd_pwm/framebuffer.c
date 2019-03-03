/*
 * FILE: framebuffer.c
 * 实现在framebuffer上画点、画线、画同心圆、清屏的函数
 */

#include <types.h>
#include "framebuffer.h"

extern unsigned int fb_base_addr;
extern unsigned int bpp;
extern unsigned int xsize;
extern unsigned int ysize;


//typedef UINT32 FB_24BPP[272][480];

/* 
 * 画点
 * 输入参数：
 *     x、y : 象素坐标
 *     color: 颜色值
 *         对于16BPP: color的格式为0xAARRGGBB (AA = 透明度),
 *     需要转换为5:6:5格式
 *         对于8BPP: color为调色板中的索引值，
 *     其颜色取决于调色板中的数值
 */
void PutPixel(UINT32 x, UINT32 y, UINT32 color)
{
    UINT8 red,green,blue;

    switch (bpp){
        case 24:
        {
            UINT32 *addr = (UINT32 *)fb_base_addr + (y * xsize + x);
            *addr =  color;
            break;
        }

        case 16:
        {
            UINT16 *addr = (UINT16 *)fb_base_addr + (y * xsize + x);
            red   = (color >> 19) & 0x1f;
            green = (color >> 10) & 0x3f;
            blue  = (color >>  3) & 0x1f;
            color = (red << 11) | (green << 5) | blue; // 格式5:6:5
            *addr = (UINT16) color;
            break;
        }
        
        case 8:
        {
            UINT8 *addr = (UINT8 *)fb_base_addr + (y * xsize + x);
            *addr = (UINT8) color;
            break;
        }

        default:
            break;
    }
}

/* 
 * 画线
 * 输入参数：
 *     x1、y1 : 起点坐标
 *     x2、y2 : 终点坐标
 *     color  : 颜色值
 *         对于16BPP: color的格式为0xAARRGGBB (AA = 透明度),
 *     需要转换为5:6:5格式
 *         对于8BPP: color为调色板中的索引值，
 *     其颜色取决于调色板中的数值
 */
void DrawLine(int x1,int y1,int x2,int y2,int color)
{
    int dx,dy,e;
    dx=x2-x1; 
    dy=y2-y1;
    
    if(dx>=0)
    {
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 1/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}   
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 2/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}   
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)

            if(dx>=dy) // 8/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}   
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 7/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}   
                    y1-=1;
                    e+=dx;
                }
            }
        }   
    }
    else //dx<0
    {
        dx=-dx;     //dx=abs(dx)
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 4/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}   
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 3/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}   
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)

            if(dx>=dy) // 5/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}   
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 6/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    PutPixel(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}   
                    y1-=1;
                    e+=dx;
                }
            }
        }   
    }
}

/* 
 * 绘制同心圆
 */
void Mire(void)
{
    UINT32 x,y;
    UINT32 color;
    UINT8 red,green,blue,alpha;

    for (y = 0; y < ysize; y++)
        for (x = 0; x < xsize; x++){
            color = ((x-xsize/2)*(x-xsize/2) + (y-ysize/2)*(y-ysize/2))/64;
            red   = (color/8) % 256;
            green = (color/4) % 256;
            blue  = (color/2) % 256;
            alpha = (color*2) % 256;

            color |= ((UINT32)alpha << 24);
            color |= ((UINT32)red   << 16);
            color |= ((UINT32)green << 8 );
            color |= ((UINT32)blue       );

            PutPixel(x,y,color);
        }
}

/* 
 * 将屏幕清成单色
 * 输入参数：
 *     color: 颜色值
 *         对于16BPP: color的格式为0xAARRGGBB (AA = 透明度),
 *     需要转换为5:6:5格式
 *         对于8BPP: color为调色板中的索引值，
 *     其颜色取决于调色板中的数值
 */
void ClearScr(UINT32 color)
{   
    UINT32 x,y;
    
    for (y = 0; y < ysize; y++)
        for (x = 0; x < xsize; x++)
            PutPixel(x, y, color);
}

#define FONTDATAMAX 2048

extern const unsigned char fontdata_8x8[FONTDATAMAX];

void lcd_putc(unsigned char c)
{
	static int x = 0;
	static int y = 0;

	int i,j;
	unsigned char line_dots;

	/* 获得字模 */
	unsigned char *char_dots = fontdata_8x8 + c * 8;	


	/* 在framebuffer里描点 */
	if (c == '\n')
	{
		//y = (y + 8) % 272;
		y += 8;
		if (y > 272)
			y = 0;
		return ;
	}
	else if (c == '\r')
	{
		x = 0;
		return;
	}

	for (i = 0; i < 8; i++)	
	{
		line_dots = char_dots[i];
		
		for (j = 0; j < 8; j++)
		{
			if (line_dots & (0x80 >> j))
			{
				PutPixel(x+j,  y+i, 0xff0000);
			}
			else
			{
				PutPixel(x+j,  y+i, 0);
			}
		}
	}

	//x = (x + 8) % 480;
	x += 8;
	if (x > 480)
		x = 0;
	
	if (x == 0)
	{
		//y = (y + 8) % 272;
		y += 8;
		if (y > 272)
			y = 0;
	}
	
}
