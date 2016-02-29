
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>


int
main(int argc, char *argv[])
{
	char msg[128];
	int sockfd;
	struct sockaddr_in serv_addr, from_addr;
	socklen_t addr_len;
	int str_len;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));


	addr_len = sizeof(from_addr);
	while (1) {
		fputs("Input message(Q/q to quit): ", stdout);
		fgets(msg, sizeof(msg), stdin);

		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
			break;

		sendto(sockfd, msg, strlen(msg), 0,
			(struct sockaddr *) &serv_addr, sizeof(serv_addr));

		memset(msg, 0, sizeof(msg));

		str_len = recvfrom(sockfd, msg, sizeof(msg), 0,
			(struct sockaddr *) &from_addr, &addr_len);
	
		msg[str_len] = '\0';
		printf("[recv %d bytes] %s\n", str_len, msg);
	}

	close(sockfd);

	return 0;
}


