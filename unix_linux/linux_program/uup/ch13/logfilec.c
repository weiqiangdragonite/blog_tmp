/*
 * logfilec.c
 * a simple logfile client - send message to logfile server
 * usage: logfile "message"
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#define oops(msg)	{ perror(msg); exit(1); }
#define SOCKNAME	"/tmp/logfilesock"

#define BUFSIZE		1024

int
main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	char *msg;
	char sockname[] = SOCKNAME;
	int sock;
	socklen_t addrlen;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <msg>\n", argv[0]);
		exit(1);
	}
	msg = argv[1];

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, sockname);
	addrlen = strlen(sockname) + sizeof(addr.sun_family);

	sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sock == -1)
		oops("socket() failed");

	if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) & addr, addrlen) == -1)
		oops("sendto() failed");

	close(sock);
	return 0;
}

