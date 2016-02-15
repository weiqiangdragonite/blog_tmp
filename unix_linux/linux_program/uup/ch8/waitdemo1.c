/*
 * waitdemo1.c
 * shows how parent pauses until child finished
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define	DELAY	2


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
	int ret;
	ret = wait(NULL);
	printf("done waiting for %d, wait() return: %d\n", pid, ret);
}

