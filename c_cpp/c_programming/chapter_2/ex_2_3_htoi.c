/*
 *
 */

#include <stdio.h>
#include <ctype.h>

int
htoi(const char s[])
{
	int i, n, c;

	if (s == NULL || s[0] == '\0')
		return 0;
	else if (s[0] == '0' && tolower(s[1]) == 'x')
		i = 2;
	else
		i = 0;

	for (n = 0; s[i] != '\0'; ++i) {
		c = tolower(s[i]);
		if (c >= '0' && c <= '9')
			n = n * 16 + (c - '0');
		else if (c >= 'a' && c <= 'f')
			n = n * 16 + (10 + c - 'a');
	}
	return n;
}


int
main(int argc, char *argv[])
{
	char s[] = "12AF";
	int val = htoi(s);
	printf("val = %d\n", val);

	return 0;
}


