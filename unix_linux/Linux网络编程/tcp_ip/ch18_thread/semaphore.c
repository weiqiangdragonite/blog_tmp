/*
 * 这个程序不是读者写者，只是从命令行读数字求和
 */



#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void *reader(void *arg);
void *writer(void *arg);

static sem_t sem_read;
static sem_t sem_write;
static int num;

int
main(int argc, char *argv[])
{
	pthread_t t1, t2;

	sem_init(&sem_read, 0, 1);
	sem_init(&sem_write, 0, 0);

	pthread_create(&t1, NULL, reader, NULL);
	pthread_create(&t2, NULL, writer, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	sem_destroy(&sem_read);
	sem_destroy(&sem_write);

	return 0;
}


void *
reader(void *arg)
{
	int i;
	for (i = 0; i < 5; ++i) {
		printf("Input num: ");

		sem_wait(&sem_read);
		scanf("%d", &num);
		sem_post(&sem_write);
	}

	return NULL;
}

void *
writer(void *arg)
{
	int i, sum = 0;
	for (i = 0; i < 5; ++i) {
		sem_wait(&sem_write);
		sum += num;
		sem_post(&sem_read);
	}
	printf("sum = %d\n", sum);

	return NULL;
}


