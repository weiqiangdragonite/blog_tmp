/*
 * 生成随机数
 * 使用rand()函数可以生成随机数，不过，它生成的随机数是
 * 一种伪随机数。因为重复执行程序的话，得到的随机数是一样。
 * 生成随机数的函数是通过一个固定的数值来计算随机数的，
 * 我们可以通过srand()函数来指定这个数值，这样生成的
 * 随机数就是真正意义上的随机数了。
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
	srand(time(NULL));
	/* return [0, RAND_MAX] */
	int num = rand();
	printf("num = %d, RAND_MAX = %d\n", num, RAND_MAX);

	/* return [0, 10) */
	num = rand() % 10;
	printf("num = %d\n", num);

	/* return [10, 100] */
	num = rand() % 91 + 10;
	printf("num = %d\n", num);

	return 0;
}

