/*
 * ticker_demo.c
 * demonstrates use of interval timer to generate reqular
 * signals, which are in turn caught and used to count down
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>


int
main(int argc, char *argv[])
{
	void countdown(int);

	signal(SIGALRM, countdown);
	if (set_ticker(500) == -1)
		perror("set_ticker() failed");
	else {
		while (1)
			pause();
	}

	return 0;
}

void
countdown(int sig)
{
	static int num = 10;
	printf("%d..\n", num--);
	fflush(stdout);
	if (num < 0) {
		printf("DONE!\n");
		exit(0);
	}
}

/*
 * set_ticker(number_of_milliseconds) milisecond = 毫秒, microsecond = 微妙
 * arg in milliseconds, converted into whole seconds and microseconds
 */
int
set_ticker(int n_msecs)
{
	struct itimerval timer;
	long n_sec, n_usecs;

	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;

	timer.it_interval.tv_sec = n_sec;
	timer.it_interval.tv_usec = n_usecs;

	timer.it_value.tv_sec = n_sec;
	timer.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL, &timer, NULL);
}





