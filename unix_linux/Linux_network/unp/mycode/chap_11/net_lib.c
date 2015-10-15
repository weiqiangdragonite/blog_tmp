/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>



/*
 * get server info
 */
struct addrinfo *
host_serv(const char *hostname, const char *service, int family, int socktype)
{
	int n;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;	/* always return canoncial name */
	hints.ai_family = family;	/* AF_UNSPEC, AF_INET, AF_INET6, etc */
	hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc */

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "host_serv - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return NULL;
	}
	return res;	/* return pointer to first on linked list */
}



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
		fprintf(stderr, "tcp_connect failed\n");
		sockfd = -1;
	}

	freeaddrinfo(res);

	return sockfd;
}


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
				sizeof(on)) == -1) {
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
#define BACK_LOG 128
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


/*
 * for udp not connected client
 *
 * struct sockaddr *sa;
 * socklen_t salen;
 * udp_client(host, serv, (void **) &sa, &salen);
 */
int
udp_client(const char *hostname, const char *service, struct sockaddr **saptr,
		socklen_t *lenp)
{
	int sockfd, n;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "udp_client - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return -1;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (sockfd >= 0)
			break;	/* success */
	}

	if (rp == NULL) {
		fprintf(stderr, "udp_client failed\n");
		freeaddrinfo(res);
		return -1;
	}

	*saptr = malloc(res->ai_addrlen);
	if (*saptr == NULL) {
		fprintf(stderr, "udp_client - malloc() failed\n");
		freeaddrinfo(res);
		return -1;
	}
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(res);
	return sockfd;
}


/*
 * for udp connected client
 *
 */
int
udp_connect(const char *hostname, const char *service)
{
	int sockfd, n;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "udp_connect - getaddrinfo() failed: %s\n",
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

		close(sockfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "udp_connect failed\n");
		sockfd = -1;
	}

	freeaddrinfo(res);
	return sockfd;
}



/*
 * for udp server:
 * success return socket fd, otherwise return -1
 */
int
udp_server(const char *hostname, const char *service, socklen_t *len)
{
	int sockfd, n;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "udp_server - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return -1;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		/* If error, try next address */
		if (sockfd == -1)
			continue;

		if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* success */

		/* bind failed: close this socket and try next address */
		close(sockfd);
	}

	/* error */
	if (rp == NULL) {
		fprintf(stderr, "udp_server failed\n");
		sockfd = -1;
	}


	if (len != NULL)
		*len = res->ai_addrlen;	/* return sizeof protocol address */

	freeaddrinfo(res);
	return sockfd;
}


