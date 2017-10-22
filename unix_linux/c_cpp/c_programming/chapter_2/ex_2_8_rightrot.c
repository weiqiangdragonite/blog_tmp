/*
 * rightrot(x,n) that returns the value of the integer x rotated to the right 
 * by n positions.
 */

#include <stdio.h>
#include <ctype.h>



unsigned int
rightrot(unsigned int x, int n)
{
	/* 要右移的n位 */
	unsigned int tmp = x & (~(~0 << n));
	x = x >> n;
	x = x | (tmp << (sizeof(x) * 8 - n));

	return x;
}


int
main(int argc, char *argv[])
{
	unsigned int x = 0xFFFFFFFB;

	x = rightrot(x, 4);
	printf("x = %x\n", x);

	return 0;
}


