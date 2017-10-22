/*
 * converts a number to a character string (the inverse of atoi).
 */


void
itoa(int n, char s[])
{
	int i, sign;

	/* record sign */
	if ((sign = n) < 0)
		n = -n;

	i = 0;
	do {	/* generate digits in reverse order */
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);

	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	reverse(s);
}

