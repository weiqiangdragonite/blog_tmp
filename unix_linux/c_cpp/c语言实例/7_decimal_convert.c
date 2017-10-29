/*
 * 十进制转换
 */

#include <stdio.h>
#include <string.h>


void reverse(char *s)
{
	int len = strlen(s);
	int i, j;
	char tmp;

	for (i = 0, j = len - 1; i < j; ++i, --j) {
		tmp = s[i];
		s[i] = s[j];
		s[j] = tmp;
	}
}


int hex_convert(int num, int base, char *ch, int size)
{
	int i, n;
	switch (base) {
		case 2:
		case 8:
		case 10:
		case 16:
			break;
		default:
			fprintf(stderr, "base error\n");
			return -1;
	}

	i = 0;
	while (num != 0) {
		/* 空间不够(要有结束) */
		if (i + 2 > size) {
			fprintf(stderr, "space is not enough\n");
			return -1;
		}
		n = num % base;
		if (base == 16 && n >= 10)
			ch[i] = 'A' + n - 10;
		else
			ch[i] = n + '0';
		num = num / base;
		++i;
	}
	ch[i] = '\0';
	reverse(ch);
	return 0;
}


int main(int argc, char *argv[])
{
	char ch[100];
	int res;
	res = hex_convert(9, 2, ch, sizeof(ch));
	if (res == 0)
		printf("ch = %s\n", ch);
	return 0;
}