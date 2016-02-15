/*
 * waitdemo2.c
 * shows how parent gets child status
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define	DELAY	15


void child_code(int delay);
void parent_code(pid_t pid);

int
main(int argc, char *argv[])
{
	pid_t pid;

	printf("Before: pid is %d\n", getpid());

	if ((pid = fork()) == -1) {
		perror("fork() failed");
		exit(1);
	} else if (pid == 0) {
		child_code(DELAY);
	} else {
		parent_code(pid);
	}

	return 0;
}


void
child_code(int delay)
{
	printf("child %d here, will sleep for %d seconds\n",
		getpid(), delay);
	sleep(delay);
	printf("child done, start to exit\n");
	exit(17);
}


void
parent_code(pid_t pid)
{
	int wait_rv;
	int status;
	int high_8, low_7, bit_7;

	wait_rv = wait(&status);
	printf("done waiting for %d, wait() return: %d\n",
		pid, wait_rv);

	high_8 = status >> 8;
	low_7 = status & 0x7F;
	bit_7 = status & 0x80;

	printf("status = %d: exit = %d, sig = %d, core = %d\n",
		status, high_8, low_7, bit_7);
}

