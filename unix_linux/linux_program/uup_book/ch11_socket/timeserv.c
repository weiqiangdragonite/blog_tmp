/*
 * timeserv.c
 * a socket - based time of day server
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORTNUM		13000
#define oops(msg)	{ perror(msg); exit(1); }


int
main(int argc, char *argv[])
{
	int serv_sock, cli_sock;

	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_size;
	time_t thetime;
	FILE *sock_fp;

	/* step 1 */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		oops("socket() failed");

	/* step 2 */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORTNUM);

	if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		oops("bind() failed");

	/* step 3 */
	if (listen(serv_sock, 5) == -1)
		oops("listen() failed");

	/* main loop */
	while (1) {
		cli_size = sizeof(cli_addr);
		cli_sock = accept(serv_sock, (struct sockaddr *) &cli_addr, &cli_size);
		if (cli_sock == -1)
			oops("accept() failed");

		sock_fp = fdopen(cli_sock, "w");
		if (sock_fp == NULL)
			oops("fdopen() failed");

		thetime = time(NULL);
		fprintf(sock_fp, "The time here is... %s", ctime(&thetime));
		fclose(sock_fp);
	}

	return 0;
}


