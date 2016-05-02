

#include <stdio.h>

int
main(int argc, char *argv[])
{
	int i;
	unsigned char array[8] = { 0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12 };

	for (i = 0; i < 8; ++i)
		printf("addr: %p, val: 0x%2X\n", &array[i], array[i]);

	unsigned char *r0 = 0;
	r0 = array;
	printf("r0 = 0x%2X\n", *r0);

	unsigned short *r2 = 0;
	r2 = (unsigned short *) (array + 2);
	printf("r2 = 0x%4X\n", *r2);

	return 0;
}
