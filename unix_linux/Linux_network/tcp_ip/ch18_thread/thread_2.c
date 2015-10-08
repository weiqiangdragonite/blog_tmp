


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void *thread_main(void *arg);


int
main(int argc, char *argv[])
{
	pthread_t tid;
	int thread_param;
	void *thr_ret;

	thread_param = 5;
	if (pthread_create(&tid, NULL, thread_main, (void *) &thread_param) != 0) {
		perror("pthread_create() failed");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(tid, &thr_ret) != 0) {
		perror("pthread_join() failed");
		exit(EXIT_FAILURE);
	}


	printf("Thread return message: %s\n", (char *) thr_ret);
	printf("end of main\n");

	return 0;
}

void *
thread_main(void *arg)
{
	int i;
	int cnt;
	char *msg = NULL;

	msg = (char *) malloc(sizeof(char) * 50);
	strncpy(msg, "hello world", 50);

	cnt = *((int *) arg);
	for (i = 0; i < cnt; ++i) {
		sleep(1);
		printf("running thread\n");
	}

	return (void *) msg;
}



