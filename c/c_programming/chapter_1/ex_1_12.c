/*
 * A program counts lines, words, and characters.
 */

#include <stdio.h>

#define IN	1	/* inside a word */
#define OUT	0	/* outside a word */

int
main(int argc, char *argv[])
{
/*
	int ch;
	while ((ch = getchar()) != EOF) {
		if (ch == ' ' || ch == '\n' || ch == '\t')
			putchar('\n');
		else
			putchar(ch);
	}
*/

	int ch, state;
	long words, lines, characters;

	state = OUT;
	words = lines = characters = 0;
	while ((ch = getchar()) != EOF) {
		++characters;
		if (ch == '\n')
			++lines;

		if (ch == ' ' || ch == '\n' || ch == '\t')
			state = OUT;
		else if (state == OUT) {
			state = IN;
			++words;
		}

		if (state == IN)
			putchar(ch);
		else if (state == OUT)
			putchar('\n');
	}

	printf("characters = %ld, words = %ld, lines = %ld\n",
		characters, words, lines);

	return 0;
}
