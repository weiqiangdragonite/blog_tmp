/*
 * A library of functions to perform tasks commonly required for Internet
 * domain sockets.
 */

#ifndef INET_SOCKET_H
#define INET_SOCKET_H

#include <sys/socket.h>
#include <netdb.h>

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

int inet_connect(const char *host, const char *service, int type);

int inet_listen(const char *service, int backlog, socklen_t *addrlen);

int inet_bind(const char *service, int type, socklen_t *addrlen);

char *inet_addrstr(const struct sockaddr *addr, socklen_t addrlen);
//                   char *addr_str, size_t str_len);

#endif	/* INET_SOCKET_H */
