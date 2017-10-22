/*
 * setbits(x,p,n,y) that returns x with the n bits that begin at position p set
 * to the rightmost n bits of y, leaving the other bits unchanged.
 */

#include <stdio.h>
#include <ctype.h>



unsigned int
setbits(unsigned int x, int p, int n, unsigned int y)
{
	int tmp;

	/* y最右边n位的值 */
	y = y & (~(~0 << n));
	printf("y = %x\n", y);

	/* 得到x最右边p+1-n位的值 */
	tmp = x & (~(~0 << (p+1-n)));
	/* 将x最右边p位清零并或上y */
	x = ((x >> (p+1)) << (p+1)) | (y << (p+1-n));
	/* 将x或上原来的p+1-n位 */
	x = x | tmp;

	return x;
}


int
main(int argc, char *argv[])
{
	unsigned int x = 0xFFFFFFFF;
	unsigned int y = 0x00FF0052;

	x = setbits(x, 7, 4, y);
	printf("x = %x\n", x);

	return 0;
}


