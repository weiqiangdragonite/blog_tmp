#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

#define BACKLOG		50
#define PORT_NUM	"8888"
#define ADDRSTRLEN	(NI_MAXHOST + NI_MAXSERV + 10)


int
main(int argc, char *argv[])
{
	struct sockaddr_storage claddr;	/* IPv4 or IPv6 */
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	char addr_str[ADDRSTRLEN];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	socklen_t addrlen;
	int sfd, cfd, ret, optval;
	char *msg = "hello, world";


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;


	ret = getaddrinfo(NULL, PORT_NUM, &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
		return -1;
	}

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (sfd == -1)
			continue;

		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
			perror("setsockopt() failed: ");
			return -1;
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind socket to any address\n");
		return -1;
	}

	if (listen(sfd, BACKLOG) == -1) {
		perror("listen() failed: ");
		return -1;
	}

	freeaddrinfo(result);

	while (1) {
		addrlen = sizeof(struct sockaddr_storage);
		cfd = accept(sfd, (struct sockaddr *) &claddr, &addrlen);
		if (cfd == -1) {
			perror("accept() failed: ");
			continue;
		}

		ret = getnameinfo((struct sockaddr *) &claddr, addrlen, host,
			NI_MAXHOST, service, NI_MAXSERV, 0);
		if (ret == 0)
			snprintf(addr_str, ADDRSTRLEN, "(%s:%s)", host, service);
		else
			snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
		printf("Connection from %s\n", addr_str);

		if (write(cfd, msg, strlen(msg)) != strlen(msg))
			perror("write() failed: ");

		close(cfd);
	}
	close(sfd);

	return 0;
}