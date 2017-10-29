/*
 * 素数
 * 素数又叫质数，在大于1的自然数中，如果一个正整数除了1和它本身外，
 * 不能被其它自然数整除，那么这个数就是一个素数。
 */

#include <stdio.h>
#include <math.h>


int is_prime_v1(int num)
{
	if (num < 2)
		return 0;
	int i;
	for (i = 2; i < num; ++i) {
		if (num % i == 0)
			return 0;
	}
	return 1;
}


/*
 * 如果一个数不是素数是合数，
 * 那么一定可以由两个自然数相乘得到，
 * 其中一个大于或等于它的平方根，一个小于或等于它的平方根。
 * 并且成对出现
 */
int is_prime(int num)
{
	if (num < 2)
		return 0;
	int large = sqrt(num);
	int i;
	for (i = 2; i <= large; ++i) {
		if (num % i == 0)
			return 0;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	int i;
	printf("prime number:\n");
	for (i = 0; i <= 100; ++i) {
		if (is_prime_v1(i))
			printf("%d ", i);
	}
	printf("\n");
	for (i = 0; i <= 100; ++i) {
		if (is_prime(i))
			printf("%d ", i);
	}
	printf("\n");

	return 0;
}
