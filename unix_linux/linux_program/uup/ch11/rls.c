/*
 * rls.c
 * a client for remote directory listening service
 * Usage: ./rls hostname directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUFSIZE		1024
#define PORTNUM		15000
#define oops(msg)	{ perror(msg); exit(1); }



int
main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int n_read;
	char buf[BUFSIZE];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <host> <directory>\n", argv[0]);
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
	serv_addr.sin_port = htons(PORTNUM);

	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		oops("connect() failed");

	/* step 3 */
	if (write(sock, argv[2], strlen(argv[2])) == -1)
		oops("write() failed");
	if (write(sock, "\n", 1) == -1)
		oops("write() failed");

	while ((n_read = read(sock, buf, BUFSIZE)) > 0) {
		if (write(1, buf, n_read) == -1)
			oops("write() failed");
	}
	close(sock);

	return 0;
}



