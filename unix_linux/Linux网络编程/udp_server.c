#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

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
	int sfd, ret;
	char buf[1024];
	ssize_t bytes;


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;


	ret = getaddrinfo(NULL, PORT_NUM, &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (sfd == -1)
			continue;

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		close(sfd);
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind socket to any address\n");
		return -1;
	}

	freeaddrinfo(result);


	while (1) {
		memset(buf, 0, sizeof(buf));

		addrlen = sizeof(struct sockaddr_storage);
		bytes = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *) &claddr, &addrlen);
		if (bytes == -1) {
			perror("recvfrom() failed: ");
			continue;
		}

		ret = getnameinfo((struct sockaddr *) &claddr, addrlen, host,
			NI_MAXHOST, service, NI_MAXSERV, 0);
		if (ret == 0)
			snprintf(addr_str, ADDRSTRLEN, "(%s:%s)", host, service);
		else
			snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
		printf("Connection from %s\n", addr_str);


		printf("recv: %s\n", buf);

	}
	close(sfd);

	return 0;
}