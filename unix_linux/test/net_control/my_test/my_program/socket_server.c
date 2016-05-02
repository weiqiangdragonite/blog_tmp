

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int
main(int argc, char *argv[])
{
	int portno = 6789;
	int flag = 1;
	int sockfd, newsockfd;

	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "ERROR opening socket\n");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
		fprintf(stderr, "ERROR ON setockopt\n");
		exit(EXIT_FAILURE);
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "ERROR on binding");
		exit(EXIT_FAILURE);
	}

	listen(sockfd, 5);

	while (1) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			fprintf(stderr, "accept client failed\n");
			continue;
		}

		char *str = "hello, world\n";
		ssize_t num_bytes;
		int tmp_errno;

		while (1) {
			tmp_errno = errno;
			errno = 0;
			num_bytes = write(newsockfd, str, strlen(str));
			if (num_bytes != strlen(str)) {
				if (errno == EPIPE) {
					close(newsockfd);
					newsockfd = -1;
					errno = tmp_errno;
					printf("Client close connection.\n");
					break;
				} else
					fprintf(stderr, "write() failed: %s\n",
						strerror(errno));
			}

			sleep(5);
		}

		continue;
	}

	close(sockfd);


	return 0;
}








