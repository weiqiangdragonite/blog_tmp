/*
 *
 */

#define _BSD_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "inet_socket.h"


char addr_str[ADDRSTRLEN];

/*
 * This function creates a socket with the given socket type, and connects it
 * to the address specified by host and service. This function is designed for
 * TCP or UDP clients that need to connect their socket to a server socket.
 */
int
inet_connect(const char *host, const char *service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int socket_fd;

	/* Set the hints argument */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	/* User define socket type (SOCK_STREAM or SOCK_DGRAM) */
	hints.ai_socktype = type;
	/* ai_protocol = 0;
	   For our purposes, this field is always specified as 0, meaning that
	   the caller will accept any protocol */
	/* Allows IPv4 or IPv6 */
	hints.ai_family = AF_UNSPEC;

	/* get a list of socket address */
	if (getaddrinfo(host, service, &hints, &result) != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", strerror(errno));
		return -1;
	}

	/* Walk through returned list until we find an address structure that
	   can be used to successfully connect a socket */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		socket_fd = socket(rp->ai_family, rp->ai_socktype,
			rp->ai_protocol);

		/* If error, try next address */
		if (socket_fd == -1)
			continue;

		/* Try to connect socket */
		if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;

		/* Connect failed: close this socket and try next address */
		close(socket_fd);
	}

	/* We must free the struct addrinfo */
	freeaddrinfo(result);

	/* Return the socket_fd or -1 is error */
	return (rp == NULL) ? -1 : socket_fd;
}


/*
 * Public interfaces: inet_bind() and inet_listen()
 */
static int
inet_passive_socket(const char *service, int type, socklen_t *addrlen,
                    int is_listen, int backlog)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int socket_fd, optval;

	/* Init hints */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_socktype = type;
	hints.ai_family = AF_UNSPEC;
	/* Use wildcard IP address */
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, service, &hints, &result) != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", strerror(errno));
		return -1;
	}

	/* Walk through returned list until we find an address structure that
	   can be used to successfully connect a socket */
	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		socket_fd = socket(rp->ai_family, rp->ai_socktype,
			rp->ai_protocol);

		/* If error, try next address */
		if (socket_fd == -1)
			continue;

		if (!is_listen)
			break;

		/* For TCP server */
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
				&optval, sizeof(optval)) == -1) {
			close(socket_fd);
			freeaddrinfo(result);
			fprintf(stderr, "setsockopt() failed: %s\n",
				strerror(errno));
			return -1;
		}

		/* Bind to socket */
		if (bind(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		/* bind() failed: close this socket and try next address */
		close(socket_fd);
	}

	if (rp != NULL && is_listen) {
		/* Set listen queue */
		if (listen(socket_fd, backlog) == -1) {
			freeaddrinfo(result);
			fprintf(stderr, "listen() failed: %s\n",
				strerror(errno));
			return -1;
		}
	}

	/* Return address structure size */
	if (rp != NULL && addrlen != NULL)
		*addrlen = rp->ai_addrlen;

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : socket_fd;
}

/*
 * The inet_listen() function creates a listening stream (SOCK_STREAM)
 * socket bound to the wildcard IP address on the TCP port specified by
 * service. This function is designed for use by TCP servers.
 */
int
inet_listen(const char *service, int backlog, socklen_t *addrlen)
{
	return inet_passive_socket(service, SOCK_STREAM, addrlen, 1, backlog);
}

/*
 * The inet_bind() function creates a socket of the given type, bound to the
 * wildcard IP address on the port specified by service and type. (The socket
 * type indicates whether this is a TCP or UDP service.) This function is
 * designed (primarily) for UDP servers and clients to create a socket bound
 * to a specific address.
 */
int
inet_bind(const char *service, int type, socklen_t *addrlen)
{
	return inet_passive_socket(service, type, addrlen, 0, 0);
}


/*
 * This function converts an Internet socket address to printable form.
 */
char *
inet_addrstr(const struct sockaddr *addr, socklen_t addrlen)
//             char *addr_str, size_t str_len)
{
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	size_t str_len = sizeof(addr_str);

	memset(addr_str, 0, str_len);
	if (getnameinfo(addr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV,
			NI_NUMERICHOST | NI_NUMERICSERV) == 0)
		snprintf(addr_str, str_len, "%s, %s", host, service);
	else
		snprintf(addr_str, str_len, "?UNKNOWN?");

	/* Ensure result is null-terminated */
	addr_str[str_len - 1] = '\0';

	return addr_str;
}
