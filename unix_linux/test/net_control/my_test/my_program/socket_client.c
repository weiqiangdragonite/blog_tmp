

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

char *server_ip = "localhost";


int
main(int argc, char *argv[])
{
	int sockfd;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	int portno = 6789;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR opening socket server_ip :%s\n", server_ip);
		exit(EXIT_FAILURE);
	}

	server = gethostbyname(server_ip);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "ERROR connecting\n");
		exit(EXIT_FAILURE);
	}

	char buffer[1024];
	ssize_t num_read;
	while (1) {
		num_read = read(sockfd, buffer, 1024);
		if (num_read == -1) {
			fprintf(stderr, "read() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		printf("num_read = %d\n", (int) num_read);
		if (num_read != 0)
			printf("recv from server: %s\n", buffer);
	}

	return 0;
}
