#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	//hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_CANONNAME;

	// name or address
	//char *host = "www.baidu.com";
	//char *host = "61.172.201.239";
	// port number or service name
	//char *service = "80";

	int ret;
	if ((ret = getaddrinfo(argv[1], NULL, &hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(ret));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp->ai_canonname != NULL)
			printf("cannonname = %s\n", rp->ai_canonname);

		char hostname[NI_MAXHOST];
		char portname[NI_MAXSERV];
		ret = getnameinfo(rp->ai_addr, rp->ai_addrlen, hostname,
			sizeof(hostname), portname, sizeof(portname), 0);
		if (ret != 0)
			fprintf(stderr, "getnameinfo() failed: %s \n", gai_strerror(ret));
		else
			printf("ip:port = %s:%s\n", hostname, portname);

	}

	freeaddrinfo(result);

	return 0;
}