/*
 *
 */

#include <stdio.h>
#include <pthread.h>


/*
 * print odd number start from arg
 */
static void *
thread_odd(void *arg)
{
	int num = *((int *) arg);
	pthread_t id;

	id = pthread_self();

	if (num % 2 == 0)
		++num;
	while (1) {
		printf("thread id(%ld): %d\n", (long) id, num);
		num += 2;
	}

	return (void *) NULL;
}

/*
 * print even number start from arg
 */
static void *
thread_even(void *arg)
{
	int num = *((int *) arg);
	pthread_t id;

	id = pthread_self();

	if (num % 2 != 0)
		++num;
	while (1) {
		printf("thread id(%ld): %d\n", (long) id, num);
		num += 2;
	}

	return (void *) NULL;
}

/*
 * calculate
 */
static void *
thread_cal(void *arg)
{
	unsigned long long i = 0;

	while (1) {
		++i;
	}

	return (void *) NULL;
}


int
main(void)
{
	pthread_t t1, t2, t3;
	int s = 0;

	s = pthread_create(&t1, NULL, thread_odd, &s);
	if (s != 0)
		fprintf(stderr, "create t1 failed\n");

	s = pthread_create(&t2, NULL, thread_even, &s);
	if (s != 0)
		fprintf(stderr, "create t2 failed\n");

	s = pthread_create(&t3, NULL, thread_cal, 0);
	if (s != 0)
		fprintf(stderr, "create t3 failed\n");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);

	return 0;
}



