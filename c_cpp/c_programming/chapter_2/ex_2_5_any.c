/*
 * returns the first location in a string s1 where any character from the
 * string s2 occurs, or -1 if s1 contains no characters from s2.
 */

#include <stdio.h>
#include <ctype.h>



int
any(char s[], char t[])
{
	int i, j, k;

	for (i = 0; s[i] != '\0'; ++i) {
		for (j = 0; t[j] != '\0'; ++j) {
			if (s[i] == t[j])
				return i;
		}
	}
	return -1;
}


int
main(int argc, char *argv[])
{
	char s[] = "hello, world. I love c programming!";
	char t[] = "bf";
	int val = any(s, t);

	printf("val = %d\n", val);

	return 0;
}


