/*
 * A program to count blanks, tabs, and newlines. 
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int ch;
	long blanks, tabs, newlines;

	blanks = tabs = newlines = 0;

	while ((ch = getchar()) != EOF) {
		if (ch == ' ')
			++blanks;
		else if (ch == '\t')
			++tabs;
		else if (ch == '\n')
			++newlines;
	}

	printf("blanks = %ld, tabs = %ld, newlines = %ld\n",
		blanks, tabs, newlines);

	return 0;
}
