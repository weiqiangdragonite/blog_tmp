#include <stdio.h>
#include <string.h>

static void *
my_memcpy(void *dest, const void *src, size_t n)
{
	char *tmp = dest;
	const char *s = src;

	while (n--) {
		*tmp = *s;
		++tmp;
		++s;
	}

	return dest;	
}

int
main(void)
{
	unsigned int a[10];
	unsigned int b[10];
	int i;

	memset(a, 0, sizeof(a));
	memset(b, 0, sizeof(b));

	for (i = 0; i < 10; ++i)
		a[i] = i + 1;

	my_memcpy(b, a, sizeof(a));

	for (i =0; i < 10; ++i)
		printf("a[%d] = %d, b[%d] = %d\n", i, a[i], i, b[i]);

	printf("\n");

	printf("&a[5] = %p, a + 5 = %p\n", &a[5], a + 5);

	return 0;
}
