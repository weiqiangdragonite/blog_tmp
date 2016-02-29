
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>


void
read_routine(int fd)
{
	char buf[1024];
	int str_len;

	while (1) {
		str_len = read(fd, buf, sizeof(buf));
		if (str_len < 0) {
			perror("read() failed");
			return;
		} else if (str_len == 0)
			return;

		buf[str_len] = '\0';
		printf("[recv %d bytes] %s\n", str_len, buf);
	}
}

void
write_routine(int fd)
{
	char buf[1024];

	while (1) {
		fputs(">>> ", stdout);
		fgets(buf, sizeof(buf), stdin);

		if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
			shutdown(fd, SHUT_WR);
			return;
		}

		write(fd, buf, strlen(buf));
	}
}


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;

	pid_t pid;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
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

	pid = fork();
	if (pid < 0) {
		perror("fork() failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		write_routine(sockfd);
	} else {
		read_routine(sockfd);
	}

	close(sockfd);

	return 0;
}


