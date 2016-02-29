/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <syslog.h>

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

int daemon_init(const char *pname, int facility);
int tcp_listen(const char *hostname, const char *service, socklen_t *len);
char *inet_addrstr(const struct sockaddr *addr, socklen_t addrlen,
	char *addr_str, size_t str_len);


int
main(int argc, char *argv[])
{
	int listenfd, connfd;
	socklen_t addrlen, len;
	struct sockaddr cliaddr;
	char buf[1024];
	time_t ticks;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s [<host>] <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	daemon_init(argv[0], 0);

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);

	while (1) {
		len = addrlen;
		connfd = accept(listenfd, &cliaddr, &len);
		char addr_str[ADDRSTRLEN];
		syslog(LOG_INFO, "connection from client %s\n",
			inet_addrstr(&cliaddr, len, addr_str, sizeof(addr_str)));

		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));

		write(connfd, buf, strlen(buf));

		close(connfd);
	}
	close(listenfd);
	return 0;
}

