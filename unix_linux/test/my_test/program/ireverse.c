#include <stdio.h>
#include <stdlib.h>

int
ireverse(int value)
{
	char str[12] = { '\0' };
	char *ptr = str;
	int tmp;
	
	while (value) {
		tmp = value %10;
		*ptr++ = tmp + '0';
		printf("tmp = %d\n", tmp);
		value /= 10;
	}

	return atoi(str);
}

int
main(void)
{
	int value = 128;
	printf("before = %d, after = %d\n", value, ireverse(value));

	return 0;
}
