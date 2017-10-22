/*
 * A program to copy its input to its output, replacing each tab by \t, each
 * backspace by \b, and each backslash by \\. This makes tabs and backspace
 * visible in an unambiguous way.
 */

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int ch;

	while ((ch = getchar()) != EOF) {
		if (ch == '\t')
			printf("\\t");
		else if (ch == '\b')
			printf("\\b");
		else if (ch == '\\')
			printf("\\\\");
		else
			putchar(ch);
	}

	return 0;
}
