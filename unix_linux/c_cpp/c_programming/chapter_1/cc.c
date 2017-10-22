/*
 * count characters in input
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	long int nc;

	nc = 0;
	while (getchar() != EOF )
		++nc;
	printf("%.0f\n", nc);

	return 0;
}


