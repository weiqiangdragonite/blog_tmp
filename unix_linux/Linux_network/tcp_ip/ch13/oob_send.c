
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("connect() failed");
		exit(EXIT_FAILURE);
	}

	write(sockfd, "123", strlen("123"));
	send(sockfd, "4", strlen("4"), MSG_OOB);

	write(sockfd, "567", strlen("567"));
	send(sockfd, "890", strlen("890"), MSG_OOB);

	close(sockfd);

	return 0;
}


