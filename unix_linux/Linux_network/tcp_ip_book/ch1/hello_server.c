
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
	int sockfd, clifd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;
	int ret;

	char msg[] = "hello, world!\n"; /* 15 bytes, include '\0' */

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

	//ret = write(clifd, msg, sizeof(msg));
	//if (ret == -1)
	//	perror("write() failed");
	//printf("write %d bytes\n", ret);

	usleep(3500);

	char str[] = "abcdefg,hijklmn, opq, rst, uvw, xyz";
	ret = write(clifd, str, sizeof(str));

	close(clifd);
	close(sockfd);

	return 0;
}

