/*
 * converts upper case letters to lower case
 */

#include <stdio.h>



char
lower(char ch)
{
	return (ch >= 'A' && ch <= 'Z') ? (ch + 'a' - 'A') : ch;
}


int
main(int argc, char *argv[])
{
	int i;
	char s[] = "HELLO, World!\n";
	for (i = 0; i < sizeof(s); ++i)
		putchar(lower(s[i]));

	return 0;
}


