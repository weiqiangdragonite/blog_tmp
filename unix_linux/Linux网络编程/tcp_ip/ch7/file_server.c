

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>

int
main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;
	int sockfd, clifd, fd;
	char buf[10];
	int bytes;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open("file_server.c", O_RDONLY);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
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

	if (listen(sockfd, 5) == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	cli_len = sizeof(cli_addr);
	clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
	if (clifd == -1) {
		fprintf(stderr, "accept() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		bytes = read(fd, buf, sizeof(buf) - 1);
		buf[bytes] = '\0';

		if (bytes == 0) { 
			break;
		} else if (bytes < sizeof(buf) - 1) {
			break;
		}
		write(clifd, buf, bytes);
	}

	shutdown(clifd, SHUT_WR);

	bytes = read(clifd, buf, sizeof(buf) - 1);
	buf[bytes] = '\0';
	printf("[recv] %s\n", buf);

	close(fd);
	close(clifd);
	close(sockfd);

	return 0;
}




