/*
 * A program to copy its input to its output, replacing each string of one or
 * more blanks by a single blank.
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int old, current;

	old = 'A';
	while ((current = getchar()) != EOF) {
		if (current != ' ' || old != ' ')
			putchar(current);

		old = current;
	}

	return 0;
}
