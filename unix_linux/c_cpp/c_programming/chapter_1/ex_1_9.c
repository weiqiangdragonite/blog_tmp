/*
 * A program to copy its input to its output, replacing each string of one or
 * more blanks by a single blank.
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int old_ch, ch;

	ch = old_ch = 0;
	while ((ch = getchar()) != EOF) {
		if (ch != ' ' || old_ch != ' ')
			putchar(ch);

		old_ch = ch;
	}

	return 0;
}
