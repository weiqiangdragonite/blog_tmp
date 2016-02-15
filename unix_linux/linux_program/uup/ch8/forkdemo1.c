/*
 * fordemo1.c
 * shows how fork creates two process, distinguishable
 * by the different return values from fork()
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	pid_t pid, ret;

	pid = getpid();
	printf("Before: my pid is %d\n", pid);

	ret = fork();
	sleep(1);
	printf("After: my pid is %d, fork() said %d\n", getpid(), ret);

	return 0;
}




