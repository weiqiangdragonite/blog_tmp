
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void
timeout(int sig)
{
	if (sig == SIGALRM)
		printf("Time out!\n");
	alarm(2);
}


int
main(int argc, char *argv[])
{
	int i;
	struct sigaction action;

	action.sa_handler = timeout;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigaction(SIGALRM, &action, 0);

	alarm(2);

	for (i = 0; i < 3; ++i) {
		printf("wait...\n");
		sleep(100);
	}

	return 0;
}




