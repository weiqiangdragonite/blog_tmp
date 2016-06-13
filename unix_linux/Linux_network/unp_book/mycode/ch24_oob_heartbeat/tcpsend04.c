/*
 * send out-of-band data
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>


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



int
main(int argc, char *argv[])
{
	int sockfd;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		return -1;
	}

	if ((sockfd = tcp_connect(argv[1], argv[2])) == -1)
		return -1;

	write(sockfd, "123", 3);
	printf("wrote 3 bytes of normal data\n");


	/* oob data */
	send(sockfd, "4", 1, MSG_OOB);
	printf("wrote 1 byte of OOB data\n");


	write(sockfd, "56", 2);
	printf("wrote 2 bytes of normal data\n");




	close(sockfd);

	return 0;
}




