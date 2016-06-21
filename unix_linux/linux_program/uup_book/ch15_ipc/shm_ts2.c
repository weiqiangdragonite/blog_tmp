/*
 * shm_ts2.c - the time server using shared memory
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

int sem_id;
int shm_id;


void cleanup(int n);
void set_sem_value(int sem_id, int semnum, int val);
void wait_and_lock(int sem_id);
void release_lock(int sem_id);


int
main(int argc, char *argv[])
{
	void *shared_memory = NULL;
	time_t now;
	int i;


	/* create shared memory */
	shm_id = shmget(MEM_KEY, SEG_SIZE, IPC_CREAT | 0777);
	if (shm_id == -1)
		oops("shmget() failed", 1);

	/* attach to it */
	shared_memory = shmat(shm_id, NULL, 0);
	if (shared_memory == NULL)
		oops("shmat() failed", 2);



	/* create semaphore, key = 9900, 2 semaphores, and mode = rw-rw-rw- */
	sem_id = semget(SEM_KEY, 2, 0666 | IPC_CREAT | IPC_EXCL);
	if (sem_id == -1)
		oops("semget() failed", 3);

	/* set couter to zero */
	set_sem_value(sem_id, 0, 0);
	set_sem_value(sem_id, 1, 0);

	signal(SIGINT, cleanup);



	/* run for a minute */
	for (i = 0; i < 60; ++i) {
		time(&now);

		printf("\tshm_ts2 waiting for lock\n");
		wait_and_lock(sem_id);	/* lock memory */

		printf("\tshm_ts2 updating memory\n");
		strcpy((char *) shared_memory, ctime(&now));	/* write to mem */
		sleep(5);

		release_lock(sem_id);	/* unlock */
		printf("\tshm_ts2 released lock\n");

		sleep(1);
	}

	cleanup(0);

	return 0;
}

void
cleanup(int n)
{
	shmctl(shm_id, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
}


/*
 * initialize semaphore
 */
void
set_sem_value(int sem_id, int semnum, int val)
{
	union semun intival;

	intival.val = val;
	if (semctl(sem_id, semnum, SETVAL, intival) == -1)
		oops("semctl", 4);
}



void
wait_and_lock(int sem_id)
{
	struct sembuf actions[2];

	actions[0].sem_num = 0;		/* sem[0] is n_readers */
	actions[0].sem_op = 0;		/* wait till no readers */
	actions[0].sem_flg = SEM_UNDO;	/* auto cleanup */

	actions[1].sem_num = 1;		/* sem[1] is n_writers */
	actions[1].sem_op = 1;		/* increase num writers */
	actions[1].sem_flg = SEM_UNDO;	/* auto cleanup */

	if (semop(sem_id, actions, 2) == -1)
		oops("semop: locking", 10);

}

void
release_lock(int sem_id)
{
	struct sembuf actions[1];

	actions[0].sem_num = 1;		/* sem[0] is n_writers */
	actions[0].sem_op = -1;		/* decrease num writers */
	actions[0].sem_flg = SEM_UNDO;	/* auto cleanup */

	if (semop(sem_id, actions, 1) == -1)
		oops("semop: unlocking", 10);
}



