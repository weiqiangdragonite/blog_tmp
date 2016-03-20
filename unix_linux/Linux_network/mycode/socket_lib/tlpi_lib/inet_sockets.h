/*
 * A library of functions to perform tasks commonly required for Internet
 * domain sockets.
 *
 * From book "The Linux Programming Interface"
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/12
 */

#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H

#include <sys/socket.h>
#include <netdb.h>

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)


/*
 * This function creates a socket with the given socket type, and connects it
 * to the address specified by host and service. This function is designed for
 * TCP or UDP clients that need to connect their socket to a server socket.
 *
 * Success return socket fd, otherwise return -1.
 */
int inet_connect(const char *host, const char *service, int type);


/*
 * The inet_listen() function creates a listening stream (SOCK_STREAM)
 * socket bound to the wildcard IP address on the TCP port specified by
 * service. This function is designed for use by TCP servers.
 *
 * Success return socket fd, otherwise return -1.
 */
int inet_listen(const char *service, int backlog, socklen_t *addrlen);


/*
 * The inet_bind() function creates a socket of the given type, bound to the
 * wildcard IP address on the port specified by service and type. (The socket
 * type indicates whether this is a TCP or UDP service.) This function is
 * designed (primarily) for UDP servers and clients to create a socket bound
 * to a specific address.
 *
 * Success return socket fd, otherwise return -1.
 */
int inet_bind(const char *service, int type, socklen_t *addrlen);


/*
 * This function converts an Internet socket address to printable form.
 *
 * If success, return charter string "(host, port)" with null terminated,
 * otherwise, return "(?UNKNOWN?)"
 */
char *inet_addrstr(const struct sockaddr *addr, socklen_t addrlen,
			char *addr_str, size_t str_len);

#endif	/* INET_SOCKET_H */
