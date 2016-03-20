/*
 * logfiled.c
 * a simple logfile server using UNIX Domain Datagram sockets
 * usage: logfiled >> logfilename
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
	char buf[BUFSIZE];
	char sockname[] = SOCKNAME;
	char *time_str;
	int num, n, sock;
	socklen_t addrlen;
	time_t now;

	num = 0;

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, sockname);
	addrlen = strlen(sockname) + sizeof(addr.sun_family);

	sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sock == -1)
		oops("socket() failed");

	if (bind(sock, (struct sockaddr *) &addr, addrlen) == -1)
		oops("bind() failed");

	/* read and write */
	while (1) {
		n = read(sock, buf, BUFSIZE);
		buf[n] = '\0';

		time(&now);
		time_str = ctime(&now);
		time_str[strlen(time_str) - 1] = '\0';

		printf("[%5d] %s %s\n", num++, time_str, buf);
		fflush(stdout);
	}
	close(sock);
	return 0;
}

