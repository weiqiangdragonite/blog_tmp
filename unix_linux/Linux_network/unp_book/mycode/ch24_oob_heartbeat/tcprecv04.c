/*
 * recv OOB data
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>




/*
 * for tcp server: create tcp socket, bind to server, listen for request
 * success return socket fd, otherwise return -1
 */
int
tcp_listen(const char *hostname, const char *service, socklen_t *len)
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "tcp_listen - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return -1;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		listenfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		/* If error, try next address */
		if (listenfd == -1)
			continue;

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on,
			sizeof(on)) == -1)
		{
			fprintf(stderr, "tcp_listen - setsockopt() failed: %s\n",
				strerror(errno)); 
			close(listenfd);
			freeaddrinfo(res);
			return -1;
		}


		if (bind(listenfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* success */

		/* bind failed: close this socket and try next address */
		close(listenfd);
	}

	/* error */
	if (rp == NULL) {
		fprintf(stderr, "tcp_listen failed\n");
		listenfd = -1;
	}


	if (listenfd != -1) {
#ifndef BACK_LOG
#define BACK_LOG 1024
#endif
		if (listen(listenfd, BACK_LOG) == -1) {
			fprintf(stderr, "tcp_listen - listen() failed: %s\n",
				strerror(errno));
			close(listenfd);
			listenfd = -1;
		}
	}

	if (len != NULL)
		*len = res->ai_addrlen;	/* return sizeof protocol address */

	freeaddrinfo(res);
	return listenfd;
}



int
sockatmark(int sockfd)
{
	int flag;

	if (ioctl(sockfd, SIOCATMARK, &flag) < 0)
		return -1;
	return (flag != 0);
}


int
main(int argc, char *argv[])
{
	int listenfd, connfd, n, on = 1;
	char buf[100];

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], NULL);
	else
		fprintf(stderr, "Usage: %s [<host>] <port>\n", argv[0]);

	if (listenfd == -1)
		return -1;


	setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on));


	connfd = accept(listenfd, NULL, NULL);
	sleep(5);


	while (1) {
		if (sockatmark(connfd))
			printf("at OOB mark\n");

		if ((n = read(connfd, buf, sizeof(buf) - 1)) == 0) {
			printf("received EOF\n");
			exit(0);
		}
		buf[n] = '\0';
		printf("read %d bytes: %s\n", n, buf);
	}

	close(connfd);
	close(listenfd);

	return 0;
}




