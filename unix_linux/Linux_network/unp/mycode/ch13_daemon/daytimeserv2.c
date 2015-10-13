/*
 *
 */

#include <stdio.h>
#include <netdb/inet.h>
#include <sys/socket.h>

int
main(int argc, char *argv[])
{
	int listenfd, connfd;
	socklen_t addrlen, len;
	struct sockaddr cliaddr;
	char buf[1024];
	time_t ticks;

	if (argc < 2 || argv > 3) {
		fprintf(stderr, "Usage: %s [<host>] <port>", argv[0]);
		exit(EXIT_FAILURE);
	}

	daemon_init(argv[0], 0);

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);

	while (1) {
		len = addrlen;
		connfd = accept(listenfd, cliaddr, &len);
		syslog(LOG_INFO, "connection from %s\n", );

		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));

		write(connfd, buf, strlen(buf));

		close(connfd);
	}

}

