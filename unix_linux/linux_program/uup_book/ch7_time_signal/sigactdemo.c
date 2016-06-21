/*
 * sigactdemo.c
 * purpose: show use of sigaction()
 * feature: blocks ^\ while handling ^C
 *          does not reset ^C handler, so two kill
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define INPUTLEN	100


int
main(int argc, char *argv[])
{
	void sigint_handler(int sig);

	struct sigaction handler;
	sigset_t block_sigs;
	char input[INPUTLEN];


	handler.sa_handler = sigint_handler;
	handler.sa_flags = SA_RESETHAND | SA_RESTART;


	/* build the list of block signals */
	sigemptyset(&block_sigs);
	sigaddset(&block_sigs, SIGQUIT);
	handler.sa_mask = block_sigs;	/* store block mask */


	if (sigaction(SIGINT, &handler, NULL) == -1) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	} else {
		while (1) {
			printf("\nType something: ");
			fgets(input, INPUTLEN, stdin);
			printf("You type: %s", input);
		}
	}

	return 0;
}

void
sigint_handler(int sig)
{
	printf("Received SIGINT signal, waiting...\n");
	sleep(5);
	printf("Leaving SIGINT handler\n");
}


