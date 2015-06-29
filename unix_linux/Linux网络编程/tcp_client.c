#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#define PORT_NUM	"8888"

int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	int cfd, ret;
	char buf[1024];

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "Usage: %s host\n", argv[0]);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_NUMERICSERV;


	ret = getaddrinfo(argv[1], PORT_NUM, &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (cfd == -1)
			continue;

		if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;

		close(cfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind socket to any address\n");
		return -1;
	}

	freeaddrinfo(result);

	if (read(cfd, buf, sizeof(buf)) == -1) {
		perror("read() failed: ");
	} else {
		printf("Data recv: %s\n", buf);
	}

	close(cfd);

	return 0;
}