/*
 * 测试能创建多少个线程
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *
thread(void *arg)
{
	long i = 0;
	while (1) {
		i = i + 1;
		sleep(1);
	}
}

int
main(int argc, char *argv[])
{
	pthread_t tid;
	int counter = 0;

	while (1) {
		if (pthread_create(&tid, NULL, thread, NULL) != 0) {
			perror("pthread_create() failed");
			while (1) {
				sleep(1);
			}
		}

		printf("create %d thread\n", ++counter);
	}

	return 0;
}

