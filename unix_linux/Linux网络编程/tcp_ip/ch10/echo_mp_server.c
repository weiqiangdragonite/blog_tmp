

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
sigchld_handler(int sig)
{
	pid_t pid;
	int status;

	pid = waitpid(-1, &status, WNOHANG);
	printf("remove child pid: %ld\n", pid);
}

void
echo(int fd)
{
	int str_len;
	char buf[1024];

	while ((str_len = read(fd, buf, sizeof(buf))) > 0)
		write(fd, buf, str_len);
}


int
main(int argc, char *argv[])
{
	int sockfd, clifd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t addr_len;

	pid_t pid;
	struct sigaction action;


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	action.sa_handler = sigchld_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	if (sigaction(SIGCHLD, &action, 0) == -1) {
		perror("sigaction() failed");
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
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 5) == -1) {
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}

	addr_len = sizeof(cli_addr);
	while (1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &addr_len);
		if (clifd == -1)
			continue;
		printf("new client connected...\n");

		pid = fork();
		if (pid < 0) {
			perror("fork() failed");
			close(clifd);
			continue;
		} else if (pid == 0) {
			close(sockfd);
			echo(clifd);
			close(clifd);
			_exit(EXIT_SUCCESS);
		} else {
			close(clifd);
		}
	}
	close(sockfd);

	return 0;
}


