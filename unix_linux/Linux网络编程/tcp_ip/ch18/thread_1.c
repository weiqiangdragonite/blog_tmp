


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread_main(void *arg);


int
main(int argc, char *argv[])
{
	pthread_t tid;
	int thread_param;

	thread_param = 5;
	if (pthread_create(&tid, NULL, thread_main, (void *) &thread_param) != 0) {
		perror("pthread_create() failed");
		exit(EXIT_FAILURE);
	}

	sleep(10);
	printf("end of main\n");

	return 0;
}

void *
thread_main(void *arg)
{
	int i;
	int cnt;

	cnt = *((int *) arg);
	for (i = 0; i < cnt; ++i) {
		sleep(1);
		printf("running thread\n");
	}

	return NULL;
}



