/*
 * test for oray ddns
 *
 * oray ddns server: phddns60.oray.net : 6060
 * 
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>



#define HOST		"phddns60.oray.net"
#define PORT		"6060"
#define BUF_SIZE	1024


#define AUTH_COMMAND	"AUTH ROUTER6\r\n"


int tcp_connect(const char *hostname, const char *service);



int
main(int argc, char *argv[])
{
	int sockfd;
	char buf[BUF_SIZE];
	ssize_t nbytes;

	/* create tcp socket and connect */
	if ((sockfd = tcp_connect(HOST, PORT)) == -1)
		exit(1);

	/** connected, recvice server welcome string */
	memset(buf, 0, sizeof(buf));
	if ((nbytes = read(sockfd, buf, BUF_SIZE)) == -1) {
		fprintf(stderr, "read() failed: %s\n", strerror(errno));
		exit(1);
	}
	printf("[recv server welcome string - %ld bytes]:\n%s\n\n",
		nbytes, buf);


	/** send auth request command */
	nbytes = write(sockfd, AUTH_COMMAND, sizeof(AUTH_COMMAND));
	if (nbytes != sizeof(AUTH_COMMAND)) {
		perror("write() failed");
		exit(1);
	}


	/** recv server key string */
	memset(buf, 0, sizeof(buf));
	if ((nbytes = read(sockfd, buf, BUF_SIZE)) == -1) {
		fprintf(stderr, "read() failed: %s\n", strerror(errno));
		exit(1);
	}
	printf("[recv server side key - %ld bytes]:\n%s\n\n",
		nbytes, buf);


	/* generate encoded auth string */


	close(sockfd);
	return 0;
}


/*
 * for tcp client: create tcp socket and connect to server
 * if success return socket fd, otherwise return -1
 */
int
tcp_connect(const char *hostname, const char *service)
{
	int fd, n;
	struct addrinfo hints, *res, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		/* IPv4, UPv6 */
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		fprintf(stderr, "tcp_connect - getaddrinfo() failed: %s\n",
			gai_strerror(n));
		return -1;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		/* error, try next address */
		if (fd == -1)
			continue;

		if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;	/* connect success */

		/* connect failed, try next */
		close(fd);
	}

	/* error from final socket() or connect() */
	if (rp == NULL) {
		fprintf(stderr, "tcp_connect() failed\n");
		fd = -1;
	}

	/* important */
	freeaddrinfo(res);

	return fd;
}


int
tcp_connect_ipv4(const char *hostname, const int port)
{
	struct sockaddr_in serv_addr;
	struct hostent *host = NULL;
	int fd, ret;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(hostname);
	serv_addr.sin_port = htons(port);

	if (serv_addr.sin_addr.s_addr == INADDR_NONE) {
		if ((host = gethostbyname(hostname)) == NULL) {
			herror("gethostbyname() failed");
			return -1;
		}

		serv_addr.sin_addr.s_addr =
			((struct in_addr *) host->h_addr_list[0])->s_addr;
	}

	ret = connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (ret == -1) {
		perror("connect() failed");
		close(fd);
		return -1;
	}

	return fd;
}












