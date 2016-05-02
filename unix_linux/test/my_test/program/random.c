/*
 *
 */


#include <stdio.h>
#include <time.h>


static unsigned long next = 1;

int rand(int range);
void srand(unsigned seed);

int
main(void)
{
	printf("sizeof(unsigned) = %d\n", sizeof(unsigned));

	int number;
	int i;

	for (i = 0; i < 10; ++i) {
		srand(time(NULL));
		number = rand(10);
		printf("number = %d\n", number);
		sleep(1);
	}

	return 0;
}

/* RAND_MAX assumed to be 32767 */
int
rand(int range)
{
	next = next * 1103515245 + 12345;
	return ((unsigned) (next / 65536) % range);
}

void
srand(unsigned seed)
{
	next = seed;
}
