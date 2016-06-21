/*
 * shm_tc2.c - the time client using shared memory
 * use semaphores for locking
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define MEM_KEY		99
#define SEM_KEY		9900
#define SEG_SIZE	1024
#define oops(m, x)	{ perror(m); exit(x); }

union semun {
	int		val;	/* Value for SETVAL */
	struct semid_ds	*buf;	/* Buffer for IPC_STAT, IPC_SET */
	unsigned short	*array;	/* Array for GETALL, SETALL */
	struct seminfo	*__buf;	/* Buffer for IPC_INFO (Linux-specific) */
};



void cleanup(int n);
void wait_and_lock(int sem_id);
void release_lock(int sem_id);



int
main(int argc, char *argv[])
{
	int shmid, sem_id;
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


	/* connect to semaphore */
	sem_id = semget(SEM_KEY, 2, 0);
	if (sem_id == -1)
		oops("semget() failed", 3);

	printf("\tshm_tc2 waiting for lock\n");
	wait_and_lock(sem_id);
	printf("\tshm_tc2 reading memory\n");

	printf("The time, direct from memory: %s", (char *) shared_memory);

	release_lock(sem_id);
	printf("\tshm_tc2 released lock\n");

	/* remove it */
	if (shmdt(shared_memory) == -1)
		oops("shmdt() failed", 3);

	return 0;
}



void
wait_and_lock(int sem_id)
{
	struct sembuf actions[2];

	actions[0].sem_num = 0;		/* sem[0] is n_readers */
	actions[0].sem_op = 1;		/* increase readers */
	actions[0].sem_flg = SEM_UNDO;	/* auto cleanup */

	actions[1].sem_num = 1;		/* sem[1] is n_writers */
	actions[1].sem_op = 0;		/* wait for no writers */
	actions[1].sem_flg = SEM_UNDO;	/* auto cleanup */

	if (semop(sem_id, actions, 2) == -1)
		oops("semop: locking", 10);

}

void
release_lock(int sem_id)
{
	struct sembuf actions[1];

	actions[0].sem_num = 0;		/* sem[0] is n_readers */
	actions[0].sem_op = -1;		/* decrease num readers */
	actions[0].sem_flg = SEM_UNDO;	/* auto cleanup */

	if (semop(sem_id, actions, 1) == -1)
		oops("semop: unlocking", 10);
}





