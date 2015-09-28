
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int
main(int argc, char *argv[])
{
	int sockfd, fd;
	struct sockaddr_in serv_addr;
	int str_len;
	char msg[128];

	if (argc != 3) {
		fprintf(stderr, "Usage: ./%s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

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

	while ((str_len = read(sockfd, msg, sizeof(msg))) != 0)
		write(fd, msg, str_len);

	write(sockfd, "ok", 2);
	close(fd);
	close(sockfd);

	return 0;
}


