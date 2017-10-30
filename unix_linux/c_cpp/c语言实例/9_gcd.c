/*
 * 最大公约数(Greatest Common Divisor)
 * 最大公因数，也称最大公约数、最大公因子，
 * 指两个或多个整数共有约数中最大的一个。
 *
 * 最大公约数的求法有好几种，最常用的是辗转相除法，也叫欧几里德算法。
 * 该算法的原理如下：
 * 1.如果正整数a除以正整数b所得的余数为c，那么a和b的最大公约数
 * 与b和c的最大公约数相等。
 * 2.重复步骤1中的操作，继续求b和c的最大公约数。如此反复，直到余数等于0为止。
 */

#include <stdio.h>

int gcd(int n1, int n2)
{
	int n3;
	while (n2) {
		n3 = n1 % n2;
		n1 = n2;
		n2 = n3;
	}
	return n1;
}


int main(int argc, char *argv[])
{
	printf("num = %d\n", gcd(1071, 462));
	printf("num = %d\n", gcd(1125, 855));
	printf("num = %d\n", gcd(319, 377));

	return 0;
}
