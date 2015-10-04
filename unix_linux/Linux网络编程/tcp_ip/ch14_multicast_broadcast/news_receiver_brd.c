

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in broad_addr;
	int str_len, ret;
	char buf[128];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&broad_addr, 0, sizeof(broad_addr));
	broad_addr.sin_family = AF_INET;
	broad_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	broad_addr.sin_port = htons(atoi(argv[1]));

	ret = bind(sockfd, (struct sockaddr *) &broad_addr, sizeof(broad_addr));
	if (ret == -1) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}


	while (1) {
		str_len = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
		if (str_len < 0)
			break;
		buf[str_len] = '\0';
		printf("[recv] %s\n", buf);
	}
	close(sockfd);

	return 0;
}


