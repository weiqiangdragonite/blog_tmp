/*
 * Print val = getchar() != EOF
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int val;

	val = getchar() != EOF;
	printf("val = getchar() != EOF\n");
	printf("val = %d\n", val);

	return 0;
}
