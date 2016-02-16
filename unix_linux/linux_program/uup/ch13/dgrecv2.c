/*
 * dgrecv2.c - datagram receiver
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }


int make_dgram_server(int portnum);
void sys_who_called(struct sockaddr_in *p);


int
main(int argc, char *argv[])
{
	int sock;
	char buf[BUFSIZE], reply[BUFSIZE];
	int msg_len;
	socklen_t cli_len;
	struct sockaddr_in cli_addr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	sock = make_dgram_server(atoi(argv[1]));
	if (sock == -1)
		oops("make_dgram_server() failed");

	while (1) {
		cli_len = sizeof(cli_addr);
		msg_len = recvfrom(sock, buf, BUFSIZE, 0,
			(struct sockaddr *) &cli_addr, &cli_len);

		buf[msg_len] = '\0';
		printf("dgrecv: got a message %s\n", buf);
		sys_who_called(&cli_addr);

		snprintf(reply, BUFSIZE, "Thank you for %d char message\n", msg_len);
		sendto(sock, reply, strlen(reply), 0,
			(struct sockaddr *) &cli_addr, sizeof(cli_addr));
	}
	close(sock);

	return 0;
}

int
make_dgram_server(int portnum)
{
	int serv_sock;
	struct sockaddr_in serv_addr;

	/* step 1 */
	serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (serv_sock == -1)
		return -1;

	/* step 2 */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portnum);

	if (bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		return -1;

	return serv_sock;
}


void
sys_who_called(struct sockaddr_in *p)
{
	char host[BUFSIZE];

	strncpy(host, inet_ntoa(p->sin_addr), BUFSIZE);

	printf("from %s: %d\n", host, ntohs(p->sin_port));
}

