/*
 * fordemo3.c
 * shows how the return values from fork()
 * allows a process to determine whether
 * it is a child or process
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
	if (ret == -1) {
		perror("fork() failed");
		exit(1);
	} else if (ret == 0) {
		printf("I'm child proces, my pid = %d\n", getpid());
	} else {
		printf("I'm parent, my child is %d\n", ret);
	}

	return 0;
}




