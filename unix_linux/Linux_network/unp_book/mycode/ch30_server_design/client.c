/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>


#define MAXN		16384	/* max bytes to reply from server */
#define MAXLINE		1024


/*
 * for tcp client: create tcp socket and connect to server
 * if success return socket fd, otherwise return -1
 */
int
tcp_connect(const char *hostname, const char *service)
{
	int sockfd, n;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	/* IPv4, IPv6 */
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "tcp_connect - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return -1;
	}


	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		/* If error, try next address */
		if (sockfd == -1)
			continue;

		/* Try to connect socket */
		if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* success */

		/* Connect failed: close this socket and try next address */
		close(sockfd);
	}

	/* error from final connect() or socket() */
	if (rp == NULL) {
		fprintf(stderr, "tcp_connect() failed: %s\n", strerror(errno));
		sockfd = -1;
	}

	freeaddrinfo(res);

	return sockfd;
}

/*
 * read n bytes from fd
 * no buffer
 */
ssize_t
readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	size_t nread;
	char *p;

	p = (char *) vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, p, nleft)) < 0) {
			if (errno == EINTR)	/* interrupt by system */
				nread = 0;	/* and call read() again */
			else
				return -1;
		} else if (nread == 0) {
			break;	/*EOF */
		}

		nleft -= nread;
		p += nread;
	}

	return (n - nleft);	/* return n >= 0 */
}



int
main(int argc, char *argv[])
{
	int i, j, fd, nchildren, nloops, nbytes;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE], reply[MAXN];

	if (argc != 6) {
		fprintf(stderr,
			"Usage: %s <ip> <port> <children> <loops> <request>\n",
			argv[0]);
		exit(-1);
	}

	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	snprintf(request, sizeof(request), "%d\n", nbytes);/* new line at end */

	for (i = 0; i < nchildren; ++i) {
		if ((pid = fork()) == 0) {	/* child */
			for (j = 0; j < nloops; ++j) {
				fd = tcp_connect(argv[1], argv[2]);

				write(fd, request, strlen(request));

				if ((n = readn(fd, reply, nbytes)) != nbytes) {
					fprintf(stderr,
						"server return %d bytes\n", n);
				}
				close(fd);
			}
			printf("child %d done\n", i);
			exit(0);
		}
		/* parent loops around to fork() again */
	}

	while (wait(NULL) > 0)	/* parent wait for all children */
		continue;

	if (errno != ECHILD) {
		perror("wait error");
		exit(-1);
	}

	return 0;
}



