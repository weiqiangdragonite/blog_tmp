/*
 * lclnt1.c
 * Liscese server client version 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE		1024
#define oops(msg)	{ perror(msg); exit(1); }

void setup(void);
void shut_down(void);
int get_ticket(void);
int release_ticket(void);

void do_regular_work(void);


int
main(int argc, char *argv[])
{
	setup();
	if (get_ticket() != 0)
		exit(1);

	do_regular_work();

	release_ticket();
	shut_down();
	return 0;
}


void
do_regular_work(void)
{
	printf("SuperSleep version 1.0 Running - License Software\n");
	sleep(10);
}








static pid_t pid = -1;
static int have_ticket = 0;
static char ticket_buf[128];
static struct sockaddr_in serv_addr;
static int serv_len;
static int sockfd;

#define PORT	20000
#define HOST	"127.0.0.1"

char *do_transaction(char *msg);
void narrate(char *msg1, char *msg2);

void
setup(void)
{
	pid = getpid();

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
		oops("socket() failed");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(HOST);
	serv_addr.sin_port = htons(PORT);

	serv_len = sizeof(serv_addr);
}

void
shut_down(void)
{
	close(sockfd);
}



int
get_ticket(void)
{
	char buf[BUFSIZE];
	char *response;

	if (have_ticket)
		return 0;

	sprintf(buf, "HELO %d", pid);	/* compose request */

	if ((response = do_transaction(buf)) == NULL)
		return -1;

	/* parse the response and see if we got a ticket.
	 * on success, the message is: TICK ticket-string
	 * on failure, the message is: FAIL failure-msg
	 */
	if (strncmp(response, "TICK", 4) == 0) {
		strcpy(ticket_buf, response + 5);	/* grab ticket-id */
		have_ticket = 1;
		narrate("got ticket", ticket_buf);
		return 0;
	}

	if (strncmp(response, "FAIL", 4) == 0)
		narrate("Could not get ticket", response);
	else
		narrate("Unknown message:", response);

	return -1;
}

int
release_ticket(void)
{
	char buf[BUFSIZE];
	char *response;

	if (!have_ticket)		/* dont have ticket, nothing to do */
		return 0;

	sprintf(buf, "GBYE %s", ticket_buf);
	if ((response = do_transaction(buf)) == NULL)
		return -1;

	/* success: THNX info-string
	 * failure: FAIL error-string
	 */
	if (strncmp(response, "THNX", 4) == 0) {
		narrate("release ticket ok", "");
		return 0;
	}

	if (strncmp(response, "FAIL", 4) == 0)
		narrate("release failed", response);
	else
		narrate("Unknown message:", response);

	return -1;
}




char *
do_transaction(char *msg)
{
	static char buf[BUFSIZE];
	struct sockaddr_in from_addr;
	socklen_t addrlen = sizeof(from_addr);
	int ret;

	ret = sendto(sockfd, msg, strlen(msg), 0,
		(struct sockaddr *) &serv_addr, serv_len);

	ret = recvfrom(sockfd, buf, BUFSIZE, 0,
		(struct sockaddr *) &from_addr, &addrlen);

	return buf;
}


void
narrate(char *msg1, char *msg2)
{
	fprintf(stderr, "CLIENT[%d]: %s %s\n", pid, msg1, msg2);
}


