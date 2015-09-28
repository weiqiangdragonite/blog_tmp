
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int
main(int argc, char *argv[])
{
	int i;
	struct hostent *host;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <addr>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	host = gethostbyname(argv[1]);
	if (host == NULL) {
		herror("gethostbyname() failed");
		exit(EXIT_FAILURE);
	}

	printf("offical name: %s\n", host->h_name);
	for (i = 0; host->h_aliases[i]; ++i)
		printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
	printf("Addres type: %s\n",
		(host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
	for (i = 0; host->h_addr_list[i]; ++i)
		printf("IP addr %d: %s\n", i + 1,
			inet_ntoa(*(struct in_addr *) host->h_addr_list[i]));

	return 0;
}

