/*
 * 生产者程序，从共享内存写入内容
 * 先运行消费者程序，再运行生产者
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>

#include "shm_common.h"


const key_t KEY = 9999;


int
main(int argc, char *argv[])
{
	int running = 1;
	void *shared_memory = NULL;
	struct shared_use_st *shared_stuff;
	char buf[1024];
	int shmid;


	/* 1. create shared memory */
	/* 0666 权限 */
	shmid = shmget(KEY, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
	if (shmid == -1) {
		perror("shmget() failed");
		exit(EXIT_FAILURE);
	}

	/* 2. let process can use shared memory */
	shared_memory = shmat(shmid, NULL, 0);
	if (shared_memory == NULL) {
		perror("shmat() failed");
		exit(EXIT_FAILURE);
	}

	printf("Memory attached at %p\n", shared_memory);

	/* 3. producer */
	shared_stuff = (struct shared_use_st *) shared_memory;

	while (running) {
		if (shared_stuff->has_data) {
			printf("waiting for client to consume ...\n");
			sleep(1);
			continue;
		}

		printf("enter some text: ");
		fgets(buf, sizeof(buf), stdin);

		strncpy(shared_stuff->some_text, buf, TEXT_SZ);
		shared_stuff->has_data = 1;

		/* end */
		if (strncmp(buf, "end", 3) == 0)
			running = 0;

	}

	/* 4. detach shared memory */
	if (shmdt(shared_memory) == -1) {
		perror("shmdt() failed");
		exit(EXIT_FAILURE);
	}


	return 0;
}


