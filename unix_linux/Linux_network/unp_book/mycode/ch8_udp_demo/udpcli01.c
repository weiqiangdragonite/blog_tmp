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
	struct sockaddr_in svaddr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <IP Address>\n", argv[0]);
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) != 1) {
		fprintf(stderr, "inet_pton() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	str_echo(sockfd, (struct sockaddr *) &svaddr, sizeof(svaddr));

	return 0;
}


void
str_echo(int sockfd, struct sockaddr *svaddr, socklen_t addrlen)
{
	ssize_t n;
	char sendmsg[BUF_SIZE], recvmsg[BUF_SIZE];

	while (fgets(sendmsg, BUF_SIZE, stdin) != NULL) {
		sendto(sockfd, sendmsg, strlen(sendmsg), 0, svaddr, addrlen);
		n = recvfrom(sockfd, recvmsg, BUF_SIZE, 0, NULL, NULL);
		recvmsg[n - 1] = '\0';
		//fputs(recvmsg, stdout);
		printf("%s\n", recvmsg);
	}
}


