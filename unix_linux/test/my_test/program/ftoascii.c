#include <stdio.h>
#include <stdlib.h>


int
ireverse(int value)
{
	char str[12] = { '\0' };
	char *ptr = str;
	int tmp;

	while (value) {
		tmp = value % 10;
		*ptr++ = tmp + '0';
		value /= 10;
	}

	return atoi(str);
}


int
ftoascii(char *str, int len, float value)
{
	int integer;
	int frac;
	int tmp;
	int count = len;

	/* 正或负*/
	if (value < 0) {
		snprintf(str, len, "002D");
		str += 4;
		len -= 4;
		value = -value;
	}
	
	/* 整数部分 */
	integer = (int) value;
	/* 小数部分 */
	frac = (int) (10 * (value - integer));

	//printf("integer = %d, frac = %d\n", integer, frac);

	integer = ireverse(integer);

	while (integer && len > 0) {
		tmp = integer % 10;
		snprintf(str, len, "00%d", 30 + tmp);
		str += 4;
		len -= 4;
		integer /= 10;
	}

	snprintf(str, len, "002E");
	str += 4;
	len -= 4;
	snprintf(str, len, "00%d", 30 + frac);

	return count - len;
}

int
main(void)
{
	char temp[25] = { '\0' };
	float value = -22.2;
	ftoascii(temp, sizeof(temp), value);

	printf("temp = %s\n", temp);

	return 0;
}
