/*
 * 斐波那契数列
 * f(n) = f(n-1) + f(n-2) , 其中 n > 1
 * f(n) = 1 , n = 0 或者 1
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * use recursive
 */
int fibonacci_re(int n)
{
	/* base case */
	if (n == 0 || n == 1)
		return 1;
	else
		return fibonacci_re(n-1) + fibonacci_re(n-2);
}

/*
 * use loop
 */
int fibonacci_lo(int n)
{
	int *fib = (int *) calloc(n+1, sizeof(int));
	int i, num;
	fib[0] = 1;
	fib[1] = 1;
	for (i = 2; i <= n; ++i)
		fib[i] = fib[i-1] + fib[i-2];
	num = fib[n];
	free(fib);
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
		printf("use recursive fib(%d) = %d\n", n, fibonacci_re(n));
		printf("use loop fib(%d) = %d\n", n, fibonacci_lo(n));
	}

	return 0;
}
