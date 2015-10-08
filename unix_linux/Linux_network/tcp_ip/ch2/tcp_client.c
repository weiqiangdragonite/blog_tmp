
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
	struct sockaddr_in serv_addr;
	int str_len, read_len, i;
	char msg[128];

	if (argc != 3) {
		fprintf(stderr, "Usage: ./%s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// same as: socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		fprintf(stderr, "connect() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	i = 0;
	while ((read_len = read(sockfd, &msg[i++], 1))) {
		if (read_len == -1) {
			perror("read() failed");
			close(sockfd);
			exit(EXIT_FAILURE);
		}

		str_len += read_len;
	}

	msg[str_len] = '\0';
	printf("[recv %d bytes] %s\n", str_len, msg);

	close(sockfd);

	return 0;
}


