/*
 * 判断闰年的方法有两种：
 * 1、如果某年能被4整除，但是不能被100整除，那么这一年就是闰年。
 * 2、如果某年能被400整除，那么这一年就是闰年。
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * input: year
 * output: is leap year, return 1, otherwise, return 0
 */
int is_leap_year(int year)
{
	return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [year]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int year = atoi(argv[1]);
	if (year)
		printf("%s is %sleap year\n", argv[1], 
			is_leap_year(year) ? "" : "not ");

	return 0;
}
