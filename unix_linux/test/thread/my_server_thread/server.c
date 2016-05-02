/*
 * 当客户端连上服务器端后，服务器端不停地发送数据给客户端，如果客户端突然关闭socket，
 * 服务器端要检测并做好相关的处理，然后继续等待客户端连接。
 *
 *
 * Please use TAB for 8 characters for this program file, do not use SPACE!
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include "inet_socket.h"


#define SEND_PORT	"10000"
#define RECEIVE_PORT	"10001"


/* Function prototypes */
static int catch_signal(int sig, void (*handler)(int));
static void sigpipe_handler(int sig);
static void sigint_handler(int sig);

static void *update_data(void);


/* Global variables */
static int has_connection = 0;


/*
 *
 */
int
main(int argc, char *argv[])
{
	/* Ignore the EPIPE signal, if we not, when client close the socket,
	   server will get this signal, and it default action is exit */
	if (catch_signal(SIGINT, sigint_handler) == -1) {
		fprintf(stderr, "catch_signal() failed!");
		exit(EXIT_FAILURE);
	}
	if (catch_signal(SIGPIPE, sigpipe_handler) == -1) {
		fprintf(stderr, "catch_signal() failed!");
		exit(EXIT_FAILURE);
	}


	/* Create thread */
	pthread_t update_thread;
	if (pthread_create(&update_thread, NULL,
				(void *) update_data, NULL) != 0) {
		fprintf(stderr, "pthread_create() failed: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create update_thread success\n");

	while (1) {
		client_addrlen = sizeof(struct sockaddr_storage);
		client_fd = accept(socket_fd,
			(struct sockaddr *) &client_addr, &client_addrlen);
	}

	/* Wait for thread finish */
	pthread_join(update_thread, NULL);

	return 0;
}

/*
 *
 */
static int
catch_signal(int sig, void (*handler)(int))
{
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	return sigaction(sig, &action, NULL);
}

/*
 *
 */
static void
sigpipe_handler(int sig)
{
	printf("Client close socket connection\n");
}

/*
 *
 */
static void
sigint_handler(int sig)
{
	printf("\nBye ~\n");
	exit(EXIT_SUCCESS);
}

/*
 *
 */
static void *
update_data(void)
{
	/* Init the serial prot and device information */

	while (1) {
		if (!has_connection)
			continue;

		/* Get device data */

		/* Store them in the data structure
		   (use metex to lock the data ?) */
	}

	return NULL;
}

/*
 *
 */
void *
send_data(void)
{
	/* */

	return NULL;
}
