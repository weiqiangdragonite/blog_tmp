/*
 * 广播
 */

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
	int opt = 1;
	int fd, ret, str_len;
	char buf[10];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	memset(&broad_addr, 0, sizeof(broad_addr));
	broad_addr.sin_family = AF_INET;
	broad_addr.sin_addr.s_addr = inet_addr(argv[1]);
	broad_addr.sin_port = htons(atoi(argv[2]));

	ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
	if (ret == -1) {
		perror("setsockopt() failed");
		exit(EXIT_FAILURE);
	}

	fd = open("news_sender_brd.c", O_RDONLY);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	while ((str_len = read(fd, buf, sizeof(buf) - 1)) > 0) {
		buf[str_len] = '\0';
		printf("buf: %s\n", buf);
		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &broad_addr, sizeof(broad_addr));
		sleep(2);
	}
	close(fd);
	close(sockfd);

	return 0;
}


