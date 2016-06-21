/*
 * sigdemo2.c - shows how to ignore a signal
 *            - press Ctrl-\ to kill this one
 */

#include <stdio.h>
#include <signal.h>



int
main(int argc, char *argv[])
{
	int i;
	signal(SIGINT, SIG_IGN);
	printf("You can't stop me\n");
	while (1) {
		printf("haha\n");
		sleep(1);
	}
	return 0;
}


