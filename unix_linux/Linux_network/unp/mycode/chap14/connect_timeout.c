/*
 *
 */

static void
alarm_handler(int sig)
{
	return;	/* interrupt the connect() */
}


int
connect_timeout(int sockfd, const struct sockaddr *saptr, socklen_t salen, int nsec)
{
	int n;

	sighandler_t old_handler = signal(SIGALRM, alarm_handler);


	if (alarm(nsec) != 0) {
		printf("connect_timeout: alarm was already set\n");
		fflush(stdout);
	}

	if ((n = connect(sockfd, saptr, salen)) < 0) {
		close(sockfd);
		if (errno == EINTR)
			errno = ETIMEOUT;
	}

	alarm(0);	/* turn off alarm */
	signal(SIGALRM, old_handler);	/* restore previous signal handler */

	return n;
}

