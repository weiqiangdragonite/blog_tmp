/*
 * System V semaphore
 *
 * 用两个不同字符的输出来表示进入和离开临界区域。如果程序启动时带有一个参数，
 * 它将在进入和退出临界区域时打印字符X，其它程序打印字符O。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
	int		val;	/* Value for SETVAL */
	struct semid_ds	*buf;	/* Buffer for IPC_STAT, IPC_SET */
	unsigned short	*array;	/* Array for GETALL, SETALL */
	struct seminfo	*__buf;	/* Buffer for IPC_INFO (Linux-specific) */
};


const key_t KEY = 999;
static int sem_id;


static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);


int
main(int argc, char *argv[])
{
	int i;
	int sleep_time;
	char op_char = 'O';

	srand((unsigned int) time(NULL));

	/* 0666 --> 权限，低9位为权限 */
	sem_id = semget(KEY, 1, 0666 | IPC_CREAT);

	/* init semaphore */
	if (argc > 1) {
		if (set_semvalue() == -1) {
			fprintf(stderr, "initialize semaphore failed\n");
			exit(EXIT_FAILURE);
		}
		op_char = 'X';
		sleep(2);
	}


	for (i = 0; i < 10; ++i) {
		if (semaphore_p() == -1)
			exit(EXIT_FAILURE);

		printf("%c", op_char);
		fflush(stdout);

		sleep_time = rand() % 3;
		sleep(sleep_time);

		printf("%c", op_char);
		fflush(stdout);

		if (semaphore_v() == -1)
			exit(EXIT_FAILURE);

		sleep_time = rand() % 2;
		sleep(sleep_time);
	}

	printf("\n%d - finished\n", getpid());

	if (argc > 1) {
		sleep(10);
		del_semvalue();
	}

	return 0;
}


/*
 * initialize semaphore
 */
static int
set_semvalue(void)
{
	union semun sem_union;

	sem_union.val = 1;
	if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
		perror("semctl() failed");
		return -1;
	}

	return 0;
}


static void
del_semvalue(void)
{
	union semun sem_union;

	if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
		fprintf(stderr, "failed to delete semaphore\n");
}


static int
semaphore_p(void)
{
	struct sembuf sem;

	sem.sem_num = 0;
	sem.sem_op = -1;	/* --> p() */
	sem.sem_flg = SEM_UNDO;

	if (semop(sem_id, &sem, 1) == -1) {
		perror("semaphore_p() failed");
		return -1;
	}

	//printf("%d - p\n", getpid());
	//fflush(stdout);

	return 0;
}

static int
semaphore_v(void)
{
	struct sembuf sem;

	sem.sem_num = 0;
	sem.sem_op = 1;		/* --> v() */
	sem.sem_flg = SEM_UNDO;

	if (semop(sem_id, &sem, 1) == -1) {
		perror("semaphore_v() failed");
		return -1;
	}

	//printf("%d - v\n", getpid());
	//fflush(stdout);

	return 0;
}








