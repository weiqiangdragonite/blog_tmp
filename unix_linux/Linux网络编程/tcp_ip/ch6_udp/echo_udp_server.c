
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
	int sockfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;

	char msg[1024];
	int str_len;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	cli_len = sizeof(cli_addr);
	while (1) {
		str_len = recvfrom(sockfd, msg, sizeof(msg), 0,
			(struct sockaddr *) &cli_addr, &cli_len);
		printf("recv %d bytes\n", str_len);
		sendto(sockfd, msg, str_len, 0,
			(struct sockaddr *) &cli_addr, cli_len);
	}

	close(sockfd);

	return 0;
}

