/*
 * sleep1.c
 * purpose - show how sleep works
 * usage   - sleep1
 * outline - sets handler, sets alarm, pauses, then returns
 */

#include <stdio.h>
#include <signal.h>

int
main(int argc, char *argv[])
{
	void wakeup(int);

	signal(SIGALRM, wakeup);
	printf("sleep 4 seconds\n");
	alarm(4);
	pause();	/* freeze here */
	printf("Back to work\n");

	return 0;
}

void
wakeup(int sig)
{
	printf("Alarm received\n");
}

