
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


void
timeout(int sig)
{
	if (sig == SIGALRM)
		printf("Time out!\n");
	alarm(2);
}

void
keycontrol(int sig)
{
	if (sig == SIGINT)
		printf("\nCTRL+C pressed\n");
}


int
main(int argc, char *argv[])
{
	int i;

	signal(SIGALRM, timeout);
	signal(SIGINT, keycontrol);

	alarm(2);

	for (i = 0; i < 3; ++i) {
		printf("wait...\n");
		sleep(100);
	}

	return 0;
}

