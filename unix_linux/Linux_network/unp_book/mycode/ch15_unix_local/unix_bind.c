/*
 *
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

int
main(int argc, char *argv[])
{
	int sockfd;
	socklen_t len;
	struct sockaddr_un addr1, addr2;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(-1);
	}

	sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

	unlink(argv[1]);	/* OK if this fails */
	//umask(0022);

	memset(&addr1, 0, sizeof(addr1));
	addr1.sun_family = AF_LOCAL;
	strncpy(addr1.sun_path, argv[1], sizeof(addr1.sun_path) - 1);
	bind(sockfd, (struct sockaddr *) &addr1, SUN_LEN(&addr1));

	len = sizeof(addr2);
	getsockname(sockfd, (struct sockaddr *) &addr2, &len);
	printf("bound name = %s, returned len = %d\n",
		addr2.sun_path, len);

	return 0;
}

