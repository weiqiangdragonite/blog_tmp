/*
GET URL HTTP/1.1\r\n
HOST: www.abc.com\r\n
若干HTTP选项\r\n
\r\n

POST URL HTTP/1.1\r\n
HOST: www.abc.com\r\n
若干HTTP选项\r\n
\r\n
POST数据段（以\r\n分隔）

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define HOST	"ip.taobao.com"	// 110.75.85.86
#define PORT	"80"

int inet_connect(const char *host, const char *service, int type);


int
main(int argc, char *argv[])
{
	int socket_fd;
	char send_buf[1024];
	unsigned char recv_buf[1024];
	ssize_t bytes;

	memset(send_buf, 0, sizeof(send_buf));
	memset(recv_buf, 0, sizeof(recv_buf));

	snprintf(send_buf, sizeof(send_buf),
		"POST /service/getIpInfo2.php HTTP/1.1\r\n"
		"Host: ip.taobao.com\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Origin: http://localhost\r\n"
		"User-Agent: Mozilla/5.0 (X11; Linux) AppleWebKit/538.1 (KHTML, like Gecko) Chrome/18.0.1025.133 Safari/538.1 Midori/0.5\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		//"Accept-Encoding: gzip, deflate\r\n"
		"Accept-Language: en-us;q=0.750\r\n"
		"Referer: http://localhost/\r\n"
		"Connection: Keep-Alive\r\n"
		//"Cookie: lzstat_uv=17740663032264063595|2857556; lzstat_ss=990659082_1_1418313139_2857556\r\n"
		"Content-Length: 7\r\n"
		"\r\n"
		"ip=myip");


	socket_fd = inet_connect("110.75.85.86", "80", SOCK_STREAM);
	if (socket_fd == -1) {
		fprintf(stderr, "Cannot connected to %s\n", HOST);
		exit(EXIT_FAILURE);
	}

	bytes = write(socket_fd, send_buf, strlen(send_buf));
	if (bytes != strlen(send_buf)) {
		fprintf(stderr, "Cannot write to %s\n", HOST);
		exit(EXIT_FAILURE);
	}


	bytes = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
	if (bytes != -1) {
		recv_buf[bytes] = '\0';
		printf("recv %ld bytes\n", bytes);
	}
	close(socket_fd);


	int index, i, j;
	for (index = 0; index < bytes; ) {
		for (i = 0; i < 16; ++index, ++i) {
			if (index < bytes)
				printf("%02x ", recv_buf[index]);
			else
				printf("   ");

			if (i == 7)
				printf(" ");
		}
		printf("    ");
		for (i = index - 16, j = 0; (i < index) && (i < bytes); ++i, ++j) {
			if (isprint(recv_buf[i]))
				printf("%c", recv_buf[i]);
			else
				printf(".");

			if (j == 7)
				printf(" ");
		}
		printf("\n");
	}
	printf("\n\n");

return 0;


/**/
	memset(send_buf, 0, sizeof(send_buf));
	memset(recv_buf, 0, sizeof(recv_buf));

	snprintf(send_buf, sizeof(send_buf),
		"GET /ic.asp HTTP/1.1\r\n"
		"Host: 1111.ip138.com\r\n"
		"\r\n");

	// 112.84.191.168
	socket_fd = inet_connect("1111.ip138.com", "80", SOCK_STREAM);
	if (socket_fd == -1) {
		fprintf(stderr, "Cannot connected to %s\n", HOST);
		exit(EXIT_FAILURE);
	}

	bytes = write(socket_fd, send_buf, strlen(send_buf));
	if (bytes != strlen(send_buf)) {
		fprintf(stderr, "Cannot write to %s\n", HOST);
		exit(EXIT_FAILURE);
	}

	bytes = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
	if (bytes != -1) {
		recv_buf[bytes] = '\0';
		printf("%s", recv_buf);
	}
	close(socket_fd);
	

	return 0;
}


/*
 * This function creates a socket with the given socket type, and connects it
 * to the address specified by host and service. This function is designed for
 * TCP or UDP clients that need to connect their socket to a server socket.
 */
int
inet_connect(const char *host, const char *service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int socket_fd;

	/* Set the hints argument */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	/* User define socket type (SOCK_STREAM or SOCK_DGRAM) */
	hints.ai_socktype = type;
	/* ai_protocol = 0;
	   For our purposes, this field is always specified as 0, meaning that
	   the caller will accept any protocol */
	/* Allows IPv4 or IPv6 */
	hints.ai_family = AF_UNSPEC;

	/* get a list of socket address */
	if (getaddrinfo(host, service, &hints, &result) != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", strerror(errno));
		return -1;
	}

	/* Walk through returned list until we find an address structure that
	   can be used to successfully connect a socket */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		socket_fd = socket(rp->ai_family, rp->ai_socktype,
			rp->ai_protocol);

		/* If error, try next address */
		if (socket_fd == -1)
			continue;

		/* Try to connect socket */
		if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;

		/* Connect failed: close this socket and try next address */
		close(socket_fd);
	}

	/* We must free the struct addrinfo */
	freeaddrinfo(result);

	/* Return the socket_fd or -1 is error */
	return (rp == NULL) ? -1 : socket_fd;
}
