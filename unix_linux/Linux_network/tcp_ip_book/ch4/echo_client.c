
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
	char buf[10];
	char msg[128];
	int sockfd;
	struct sockaddr_in serv_addr;
	int str_len, recv_len, ret;

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

	while (1) {
		fputs("Input message(Q/q to quit): ", stdout);
		fgets(msg, sizeof(msg), stdin);

		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
			break;

		str_len = write(sockfd, msg, strlen(msg));
		recv_len = 0;

		/* if buf size is less than msg size, this code is dangerous,
		so, you must insure the buf size is larger than msg size */
		while (recv_len < str_len) {
			ret = read(sockfd, &buf[recv_len], sizeof(buf) - 1);
			if (ret == -1) {
				perror("read() failed");
				exit(EXIT_FAILURE);
			}

			recv_len += ret;
		}
		buf[recv_len] = '\0';
		printf("[recv %d bytes] %s\n", recv_len, buf);
	}

	close(sockfd);

	return 0;
}


