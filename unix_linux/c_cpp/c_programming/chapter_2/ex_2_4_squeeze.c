/*
 * deletes each character in s1 that matches any character in the string s2.
 */

#include <stdio.h>
#include <ctype.h>



void
squeeze(char s[], char t[])
{
	int i, j, k;
	int is_match;

	for (i = k = 0; s[i] != '\0'; ++i) {
		for (j = is_match = 0; t[j] != '\0'; ++j) {
			if (s[i] == t[j])
				is_match = 1;
		}
		if (is_match == 0)
			s[k++] = s[i];
	}
	s[k] = '\0';
}


int
main(int argc, char *argv[])
{
	char s[] = "hello, world. I love c programming!";
	char t[] = "lo";
	squeeze(s, t);

	printf("s = %s\n", s);

	return 0;
}


