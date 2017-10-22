/*
 * A program that remove trailing blanks and tabs from each line of input, 
 * and to delete entirely blank lines.
 */

#include <stdio.h>

#define MAXLINE		1000	/* maximum input line length */


int get_line(char line[], int maxline);
int remove_ch(char line[], int length);
void copy(char to[], char from[]);


int
main(int argc, char *argv[])
{
	int len;		/* current line length */
	char line[MAXLINE];	/* current input line */

	while ((len = get_line(line, MAXLINE)) > 0) {
		len = remove_ch(line, len);
		if (len > 0)
			printf("%s", line);
	}

	return 0;
}

/*
 * getline: read a line into s, return length
 */
int
get_line(char s[], int lim)
{
	int c, i;

	for (i = 0; i < (lim - 1) && (c = getchar()) != EOF && c != '\n'; ++i)
		s[i] = c;

	if (c == '\n') {
		s[i] = c;
		++i;
	}
	s[i] = '\0';

	return i;
}

/*
 *
 */
int
remove_ch(char s[], int n)
{
	int i;
	/* n-1 = '\n' */
	for (i = n - 2; i >= 0; --i, --n) {
		if (s[i] == '\t' || s[i] == ' ')
			s[i] = '\0';
		else
			break;
	}
	return n;
}

/*
 * copy: copy 'from' into 'to'; assume 'to' is big enough
 */
void
copy(char to[], char from[])
{
	int i;

	i = 0;
	while ((to[i] = from[i]) != '\0')
		++i;
}

