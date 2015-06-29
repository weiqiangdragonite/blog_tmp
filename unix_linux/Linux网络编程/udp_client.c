#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#define PORT_NUM	"8888"

int
main(int argc, char *argv[])
{
	struct sockaddr svaddr;
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	int fd, ret;
	char *msg = "hello, world";
	int bytes;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "Usage: %s host\n", argv[0]);
		return -1;
	}

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_NUMERICSERV;


	ret = getaddrinfo(argv[1], PORT_NUM, &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (fd == -1)
			continue;

		break;
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind socket to any address\n");
		return -1;
	}

	svaddr = *(rp->ai_addr);

	freeaddrinfo(result);

	bytes = sendto(fd, msg, strlen(msg), 0, &svaddr, sizeof(struct sockaddr));

	close(fd);

	return 0;
}