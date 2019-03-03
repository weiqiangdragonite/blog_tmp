#include <stdio.h>

int max(int a, int b);

int
main(void)
{
	int (*func_ptr)(int, int);
	func_ptr = max;

	int x, y, z;

	x = 5;
	y = 4;
	z = func_ptr(x, y);
	printf("max = %d\n", z);

	return 0;
}

int
max(int a, int b)
{
	return (a >= b) ? a : b;
}
