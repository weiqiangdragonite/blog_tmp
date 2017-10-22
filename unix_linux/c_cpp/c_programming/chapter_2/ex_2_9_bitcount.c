/*
 * counts the number of 1-bits in its integer argument
 */

#include <stdio.h>
#include <ctype.h>



int
bitcount(unsigned int x)
{
	int b;
	while (x) {
		++b;
		x &= (x - 1);
	}

	return b;
}


int
main(int argc, char *argv[])
{
	unsigned int x = 0xF;
	int val = bitcount(x);
	printf("val = %d\n", val);

	return 0;
}


