/*
 * lserv1.c
 * Liscese server server program version 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }

int setup(void);
void handle_request(char *req, struct sockaddr_in *cli_addr, socklen_t cli_len);
void narrate(char *msg1, char *msg2, struct sockaddr_in *clip);


int
main(int argc, char *argv[])
{
	struct sockaddr_in cli_addr;
	socklen_t cli_len;
	char buf[BUFSIZE];
	int ret, fd;

	fd = setup();

	while (1) {
		cli_len = sizeof(cli_addr);
		ret = recvfrom(fd, buf, BUFSIZE, 0,
			(struct sockaddr *) &cli_addr, &cli_len);
		if (ret!= -1) {
			buf[ret] = '\0';
			narrate("GOT: ", buf, &cli_addr);
			handle_request(buf, &cli_addr, cli_len);
		} else if (errno != EINTR)
			perror("recvfrom() failed");
	}
	return 0;
}



#define MAXUSERS	3
int ticket_array[MAXUSERS];
int num_tickets_out = 0;
int sockfd = -1;

#define PORT		20000
#define TICKET_AVAIL	0


char *do_goodbye(char *msg);
char *do_hello(char *msg);
void free_all_tickets(void);


int
setup(void)
{
	struct sockaddr_in serv_addr;

	/* step 1 */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
		oops("socket() failed");

	/* step 2 */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
		oops("binf() failed");

	free_all_tickets();

	return sockfd;
}

void
free_all_tickets(void)
{
	int i;
	for (i = 0; i < MAXUSERS; ++i)
		ticket_array[i] = TICKET_AVAIL;
}


void
handle_request(char *req, struct sockaddr_in *cli_addr, socklen_t cli_len)
{
	char *response;
	int ret;

	if (strncmp(req, "HELO", 4) == 0)
		response = do_hello(req);
	else if (strncmp(req, "GBYE", 4) == 0)
		response = do_goodbye(req);
	else
		response = "FAIL invalid request";

	narrate("SAID: ", response, cli_addr);
	ret = sendto(sockfd, response, strlen(response), 0,
		(struct sockaddr *) cli_addr, cli_len);
	if (ret == -1)
		perror("SERVER sendto() failed");
}

char *
do_hello(char *msg)
{
	int i;
	static char reply[BUFSIZE];

	if (num_tickets_out >= MAXUSERS)
		return "FAIL no tickets a aliable";

	for (i = 0; i < MAXUSERS && ticket_array[i] != TICKET_AVAIL; ++i)
		continue;

	if (i == MAXUSERS) {
		narrate("database corrupt", "", NULL);
		return "FAIL database corrupt";
	}

	ticket_array[i] = atoi(msg + 5);	/* get pid in msg */
	sprintf(reply, "TICK %d-%d", i, ticket_array[i]);
	++num_tickets_out;

	return reply;
}


char *
do_goodbye(char *msg)
{
	pid_t pid;
	int slot;

	if ((sscanf(msg + 5, "%d-%d", &slot, &pid)) != 2 || ticket_array[slot] != pid) {
		narrate("Bogus ticket", msg+5, NULL);
		return "FAIL invalid ticket";
	}

	ticket_array[slot] = TICKET_AVAIL;
	--num_tickets_out;

	return "THNX See ya!";
}



void
narrate(char *msg1, char *msg2, struct sockaddr_in *clip)
{
	fprintf(stderr, "SERVER: %s %s ", msg1, msg2);
	fprintf(stderr, "(%s:%d)", inet_ntoa(clip->sin_addr), ntohs(clip->sin_port));
	putc('\n', stderr);
}


