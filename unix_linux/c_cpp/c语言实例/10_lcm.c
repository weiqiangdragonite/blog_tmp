/*
 * 最小公倍数(Least Common Multiple)
 * 两个或多个整数公有的倍数叫做它们的公倍数，其中除0以外
 * 最小的一个公倍数就叫做这几个整数的最小公倍数。
 *
 * 最小公倍数的求法有好几种，最常用的是通过最大公约数来求。
 * 因为两个数的最大公约数乘以它们的最小公倍数等于这两个数的积。
 * 所以通常先求出两个数的最大公约数，然后用两个数的积除以
 * 它们的最大公约数就得到了它们的最小公倍数。
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

int lcm(int n1, int n2)
{
	return n1 * n2 / gcd(n1, n2);
}


int main(int argc, char *argv[])
{
	printf("num = %d\n", lcm(45, 30));
	printf("num = %d\n", lcm(36, 270));

	return 0;
}


