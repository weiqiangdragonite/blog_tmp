/*
 * dgsend.c - datagram sender
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }


int make_dgram_server(int portnum);
void sys_who_called(struct sockaddr_in *p);


int
main(int argc, char *argv[])
{
	int sock;
	char buf[BUFSIZE];
	int msg_len;
	socklen_t cli_len;
	struct sockaddr_in cli_addr, serv_addr;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <host> <port> <msg>\n", argv[0]);
		exit(1);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
		oops("socket() failed");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));


	if (sendto(sock, argv[3], strlen(argv[3]), 0,
	(struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		oops("sendto() failed");
	close(sock);

	return 0;
}




