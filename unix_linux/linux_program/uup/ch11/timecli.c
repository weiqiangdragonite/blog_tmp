/*
 * timecli.c
 * a socket - based time of day server
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }


int
main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int mslen;
	char msg[BUFSIZE];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}

	/* step 1 */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		oops("socket() failed");

	/* step 2 */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		oops("connect() failed");

	/* step 3 */
	mslen = read(sock, msg, BUFSIZE);
	if (mslen == -1)
		oops("read() failed");
	if (write(1, msg, mslen) != mslen)
		oops("write() failed");
	close(sock);

	return 0;
}


