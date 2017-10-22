/*
 * invert(x,p,n) that returns x with the n bits that begin at position p 
 * inverted (i.e., 1 changed into 0 and vice versa), leaving the others unchanged.
 */

#include <stdio.h>
#include <ctype.h>



unsigned int
invert(unsigned int x, int p, int n)
{
	unsigned int tmp, val;
	/* 要得到x从p开始的n位的原始值 */
	tmp = ((~(~0 << n)) << (p+1-n));
	/* 取反后的值 */
	val = (~(x & tmp)) & tmp ;
	x = (x & (~tmp)) | val;

	return x;
}


int
main(int argc, char *argv[])
{
	unsigned int x = 0xFFFFFFBF;

	x = invert(x, 7, 4);
	printf("x = %x\n", x);

	return 0;
}


