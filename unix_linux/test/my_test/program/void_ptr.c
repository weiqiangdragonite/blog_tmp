#include <stdio.h>

void print_val(void *arg);
void writelines(char *lineptr[], int nlines);

char *lineptr[10];


int
main(int argc, char *argv[])
{
	int val = 10;
	int num = 20;
	//print_val(&10);
	const int *ptr;
	ptr = &val;
	print_val((void *) ptr);
	ptr = &num;
	print_val((void *) ptr);

	//
	int x = 1;
	int *ip;

	ip = &x;
	printf("++*ip = %d, x = %d\n", ++*ip, x);
	printf("x = %d, ip = %p\n", x, ip);

	x = 1;
	printf("(*ip)++ = %d, x = %d\n", (*ip)++, x);
	printf("x = %d, ip = %p\n", x, ip);

	x = 1;
	printf("*ip++ = %d, x = %d\n", *ip++, x);
	printf("x = %d, ip = %p\n", x, ip);

	printf("\n");

	lineptr[0] = "hello, 0";
	lineptr[1] = "hello, 1";
	lineptr[2] = "hello, 2";
	lineptr[3] = "hello, 3";
	lineptr[4] = "hello, 4";
	lineptr[5] = "hello, 5";
	lineptr[6] = "hello, 6";
	lineptr[7] = "hello, 7";
	lineptr[8] = "hello, 8";
	lineptr[9] = "hello, 9";

	printf("lineptr[0] = %s\n", lineptr[0]);
	printf("lineptr[0][0] = %c\n", *lineptr[1]);

	writelines(lineptr, 10);

	return 0;
}

void
writelines(char *lineptr[], int nlines)
{
	while (--nlines >= 0)
		printf("%s\n", *lineptr++);
}

void
print_val(void *arg)
{
	int val = *(int *) arg;
	printf("val = %d\n", val);
}
