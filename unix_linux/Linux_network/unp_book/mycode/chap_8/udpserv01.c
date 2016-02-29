/*
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE	1024
#define SERV_PORT	10000

void str_echo(int sockfd, struct sockaddr *cliaddr, socklen_t addrlen);

int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in svaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(SERV_PORT);

	bind(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr));

	str_echo(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));

	return 0;
}


void
str_echo(int sockfd, struct sockaddr *cliaddr, socklen_t addrlen)
{
	ssize_t n;
	char msg[BUF_SIZE];

	while (1) {
		n = recvfrom(sockfd, msg, BUF_SIZE, 0, cliaddr, &addrlen);
		sendto(sockfd, msg, n, 0, cliaddr, addrlen);
	}
}


