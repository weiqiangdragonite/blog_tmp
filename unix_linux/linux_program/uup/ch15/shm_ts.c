/*
 * shm_ts.c - the time server using shared memory
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>



#define MEM_KEY		999
#define SEG_SIZE	1024
#define oops(m, x)	{ perror(m); exit(x); }


int
main(int argc, char *argv[])
{
	int shmid;
	void *shared_memory = NULL;
	char *mem_ptr;
	time_t now;
	int n, i;

	/* create shared memory */
	shmid = shmget(MEM_KEY, SEG_SIZE, IPC_CREAT | 0777);
	if (shmid == -1)
		oops("shmget() failed", 1);

	/* attach to it */
	shared_memory = shmat(shmid, NULL, 0);
	if (shared_memory == NULL)
		oops("shmat() failed", 2);

	mem_ptr = (char *) shared_memory;

	/* run for a minute */
	for (i = 0; i < 60; ++i) {
		time(&now);
		strcpy(mem_ptr, ctime(&now));	/* write to mem */
		sleep(1);
	}

	/* remove it */
	if (shmctl(shmid, IPC_RMID, 0) == -1)
		oops("shmctl() failed", 3);

	return 0;
}
