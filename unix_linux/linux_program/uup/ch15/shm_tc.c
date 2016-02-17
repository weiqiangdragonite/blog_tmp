/*
 * shm_tc.c - the time client using shared memory
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
	time_t now;
	int n;

	/* create shared memory */
	shmid = shmget(MEM_KEY, SEG_SIZE, 0777);
	if (shmid == -1)
		oops("shmget() failed", 1);

	/* attach to it */
	shared_memory = shmat(shmid, NULL, 0);
	if (shared_memory == NULL)
		oops("shmat() failed", 2);

	printf("The time, direct from memory: %s", (char *) shared_memory);

	/* remove it */
	if (shmdt(shared_memory) == -1)
		oops("shmdt() failed", 3);

	return 0;
}
