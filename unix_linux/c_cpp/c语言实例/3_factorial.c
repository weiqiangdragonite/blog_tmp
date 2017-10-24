/*
 * 求阶乘
 * 如果使用32位int，求阶乘不能超过13，否则就会溢出，往后计算机都会直接等于0，
 * 使用64位long不能超过20。
 */

#include <stdio.h>
#include <stdlib.h>


/*
 * use recursive
 */
int factorial_re(int n)
{
	/* base case: 0! = 1, 1! = 1 */
	if (n == 0 || n == 1)
		return 1;
	else
		return n * factorial_re(n - 1);
}

/*
 * use loop
 */
int factorial_lo(int n)
{
	/* n! = n * (n-1) * (n-2) * ... * 3 * 2 * 1; */
	int i;
	int num = 1;
	for (i = 1; i <= n; ++i)
		num *= i;
	return num;
}


int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [num]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int n = atoi(argv[1]);
	if (n) {
		printf("use recursive %d!= %d\n", n, factorial_re(n));
		printf("use loop %d!= %d\n", n, factorial_lo(n));
	}

	return 0;
}