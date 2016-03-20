/*
 * 消费者程序，从共享内存读出生产者的内容
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
	int shmid;

	srand((unsigned int) time(NULL));

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

	printf("Memory shared at %p\n", shared_memory);

	/* 3. consumer */
	shared_stuff = (struct shared_use_st *) shared_memory;
	shared_stuff->has_data = 0;

	while (running) {
		if (shared_stuff->has_data) {
			printf("You wrote: %s", shared_stuff->some_text);
			sleep(rand() % 4);
			shared_stuff->has_data = 0;

			/* end consumer */
			if (strncmp(shared_stuff->some_text, "end", 3) == 0)
				running = 0;
		}
		sleep(1);
	}

	/* 4. detach shared memory */
	if (shmdt(shared_memory) == -1) {
		perror("shmdt() failed");
		exit(EXIT_FAILURE);
	}

	/* 5. delete */
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("shmctl() failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}


