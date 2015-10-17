/*
 *
 */


void
str_cli(int sockfd)
{
	int maxfd, val, stdineof;
	ssize_t n, nwritten;
	fd_set rset, wset;
	char to[MAXLINE], fr[MAXLINE];
	char *toiptr, *tooptr, *friptr, *froptr;

	val = fcntl(sockfd, G_GETFL, 0);
	fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

	val = fcntl(STDIN_FILENO, G_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

	val = fcntl(STDOUT_FILENO, G_GETFL, 0);
	fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

	toiptr = tooptr = to;	/* initialize buffer pointers */
	friptr = froptr = fr;
	stdineof = 0;

	maxfd = max(STDIN_FILENO, STDOUT_FILENO, sockfd) + 1;

	while (1) {
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		if (stdineof == 0 && toiptr < &to[MAXLINE])
			FD_SET(STDIN_FILENO, &rset);	/* read from stdin */

		if (friptr < &fr[MAXLINE])
			FD_SET(sockfd, &rset);		/* read from socket */

		if (tooptr != toiptr)
			FD_SET(sockfd, &wset);		/* datas write socket */

		if (froptr != friptr)
			FD_SET(STDOUT_FILENO, &wset);	/* datas write to stdout */


		select(maxfd, &rset, &wset, NULL, NULL);


		/* read from stdin */
		if (FD_ISSET(STDIN_FILENO, &rset)) {
			if ((n = read(STDIN_FILENO, toiptr, &to[MAXLINE] - toiptr)) < 0) {
				if (errno != EWOULDBLOCK)
					fprintf(stderr, "read error on stdin\n");
			} else if (n == 0) {
				fprintf(stderr, "%s: EOF on stdin\n", gf_time());
				stdineof = 1;
				if (tooptr == toiptr)	/* no datas need to send */
					shutdown(sockfd, SHUT_WR);	/* send FIN */
			} else {
				printf("%s: read %d butes from stdin\n", gd_time(), n);
				fflush(stdout);
				toiptr += n;
				FD_SET(sockfd, &wset);	/* try and write to socket */
			}
		}

		/* read from socket */
		if (FD_ISSET(sockfd, &rset)) {
			if ((n = read(sockfd, friptr, &fr[MAXLINE] - friptr)) < 0) {
				if (errno != EWOULDBLOCK)
					fprintf(stderr, "read error on socket\n");
			} else if (n == 0) {
				printf("%s: EOF on socket\n", gf_time());
				if (stdineof)
					return;	/* normal termination */
				else {
					fprintf(stderr, "server terminated prematurely\n");
					exit(EXIT_SUCCESS);
				}
			} else {
				printf("%s: read %d bytes from socket\n", gf_time(), n);
				friptr += n;
				FD_SET(STDOUT_FILENO, &wset);	/* try and write to stdout */
			}
		}

		/* write to stdout */
		if (FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr - froptr) > 0)) {
			if ((nwritten = write(STDOUT_FILENO, froptr, n)) < 0) {
				if (errno != EWOULDBLOCK)
					fprintf(stderr, "write error to stdout\n");
			} else {
				printf("%s: wrote %d bytes to stdout\n", gf_time(), nwritten);
				froptr += nwritten;
				if (froptr == friptr)
					froptr friptr = fr;	/* back to beginning buffer */
			}
		}

		/* write to socket */
		if (FD_ISSET(sockfd, &wset) && ((n = toiptr - tooptr) > 0)) {
			if ((nwritten = write(sockfd, tooptr, n)) < 0) {
				if (errno != EWOULDBLOCK)
					fprintf(stderr, "write error to socket\n");
			} else {
				printf("%s: wrote %d bytes to socket\n", gf_time(), nwritten);
				tooptr += nwritten;
				if (tooptr == toiptr)
					tooptr = toiptr = to;	/* back to beginning buffer */
			}
		}
	}
}


/*
 * return 12:34:56.123456 - 包含微妙
 */
char *
gf_time(void)
{
	struct timeval tv;
	static char str[30];
	char *ptr;

	if (gettimeofday(&tv.tv_sec) < 0)
		fprintf(stderr, "gettimeofday error");

	ptr = ctime(&tv.tv_sec);
	strcpy(str, &ptr[11]);
	/* Fri Sep 13 00:00:00 1986\n\0 */
	/* 012345678901234567890123 4 5 */
	snprintf(str + 8, sizeof(str) - 8, ".%06ld", tv.tv_usec);

	return str;
}


