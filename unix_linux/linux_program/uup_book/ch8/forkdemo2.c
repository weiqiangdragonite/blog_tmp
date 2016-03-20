/*
 * fordemo2.c
 * shows how child processes pick up at the return
 * from fork() and can execute any code they like,
 * even fork(). Predict number of lines of output.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	printf("Before: my pid is %d\n", getpid());

	fork();
	fork();
	fork();

	printf("After: my pid is %d\n", getpid());

	return 0;
}




