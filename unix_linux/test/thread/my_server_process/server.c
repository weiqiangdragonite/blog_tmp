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


#define SERVER_PORT	"18888"


/* Function prototypes */
void *general_func(void);
void *infrared_func(void);
void *com_func(void);
static int catch_signal(int sig, void (*handler)(int));
static void sigpipe_handler(int sig);
static void sigint_handler(int sig);


/* Global variables */
int general_fd = -1;
int infrared_fd = -1;


/*
 * Main program entry.
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


	pid_t child_pid;

	/* Fork child process */
	child_pid = fork();
	switch (child_pid) {
	case -1:
		/* Fork error */
		fprintf(stderr, "fork() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	case 0:
		/* Child proess must run until server shutdown */
		handle_connect();
		/* I think we never execute this line */
		_exit(EXIT_SUCCESS);
	default:
		/* Parent process */
		break;
	}


	/* Parent process */
	/* Create thread */
	pthread_t update_thread;

	if (pthread_create(&update_thread, NULL,
				(void *) update_data, NULL) != 0) {
		fprintf(stderr, "pthread_create() failed: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create update_thread success\n");

	/* Wait for thread finish */
	pthread_join(update_thread, NULL);


	return 0;
}

/*
 * Child process from main program. Used for connect to client.
 */
static void
handle_connect(void)
{
	struct sockaddr_storage client_addr;
	socklen_t server_addrlen, client_addrlen;
	int socket_fd, client_fd;

	socket_fd = inet_listen(SERVER_PORT, SOMAXCONN, &server_addrlen);
	if (socket_fd == -1) {
		fprintf(stderr, "inet_listen() failed\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		client_addrlen = sizeof(struct sockaddr_storage);
		client_fd = accept(socket_fd,
			(struct sockaddr *) &client_addr, &client_addrlen);

		if (client_fd == -1) {
			fprintf(stderr, "accept() client failed: %s\n",
				strerror(errno));
			continue;
		}
		printf("Get connection from client\n");

		/* Fork child process to handle client */
		child_pid = fork();
		switch (child_pid) {
		case -1:
			fprintf(stderr, "fork() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		case 0:
			/* Child process */

			/* Close parent socket */
			close(socket_fd);
			/* Handle client connection */
			handle_client_connection();
			/* Child process exit */
			_exit(EXIT_SUCCESS);
		default:
			/* Parent process */
			break;
		}

		/* Close child socket and wait for next client connect */
		close(client_fd);
	}
	/* I think we never execute this line */
	close(socket_fd);
}

/*
 * Child process form handle_connect().
 */
static void
handle_client_connection(void)
{
	pthread_t send_thread;
	pthread_t receive_thread;

	if (pthread_create(&send_thread, NULL,
				(void *) send_data, NULL) != 0) {
		fprintf(stderr, "Create send_thread failed: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create send_thread success\n");

	if (pthread_create(&receive_thread, NULL,
				(void *) receive_data, NULL) != 0) {
		fprintf(stderr, "Create receive_thread() failed: %s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create receive_thread success\n");

	pthread_join(send_thread, NULL);
	pthread_join(receive_thread, NULL);
}

/*
 * From main program thread.
 */
void *
update_data(void)
{
	/* Init the serial prot and device information */

	/* Get device data */

	/* Store them in the data structure (use metex to lock the data ?) */

	return NULL;
}

/*
 * From child process.
 */
void *
send_data(void)
{
	/* */

	return NULL;
}

void *
receive_data(void)
{
	/* */

	return NULL;
}









void *
com_func(void)
{
	ssize_t num_bytes;
	char temperature[1024];
	char ups[1024];
	char controller[1024];

	
	snprintf(temperature, sizeof(temperature), "01#%3.0f#%3.1f#%3.1f\\r\\n", 1.0, 30.0, 70.0);
	snprintf(controller, sizeof(controller), "02#%3.0f#%1.0f#%1.0f#%1.0f#%1.0f#%1.0f#%1.0f#%1.0f#%1.0f\\r\\n", 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
	snprintf(ups, sizeof(ups), "03#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f\\r\\n", 220.0, 220.0, 220.0, 220.0, 220.0, 220.0, 220.0, 220.0);


	/*
	snprintf(temperature, sizeof(temperature), "01|##23##20##\\r\\n");
	snprintf(ups, sizeof(ups), "02|##220##220##\\r\\n");
	snprintf(controller, sizeof(controller), "03|##1##0##\\r\\n");
	*/

	int i = 0;
	while (1) {
		if (general_fd == -1) {
			printf("waiting for client to connect\n");
			sleep(5);
			continue;
		}

		//
		sleep(1);
		if (i == 0) {
			snprintf(ups, sizeof(ups), "03#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f\\r\\n", 222.0, 222.0, 222.0, 222.0, 222.0, 222.0, 222.0, 222.0);
			i = 1;
		} else {
			snprintf(ups, sizeof(ups), "03#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f#%3.0f\\r\\n", 220.0, 220.0, 220.0, 220.0, 220.0, 220.0, 220.0, 220.0);
			i = 0;
		}

		//
		num_bytes = write(general_fd, temperature, strlen(temperature));
		if (num_bytes != strlen(temperature)) {
			fprintf(stderr, "write temperature failed: %s\n",
				strerror(errno));
			continue;
		}
		printf("send %d bytes to client\n", num_bytes);
		usleep(100);

		//
		num_bytes = write(general_fd, ups, strlen(ups));
		if (num_bytes != strlen(ups)) {
			fprintf(stderr, "write ups failed: %s\n",
				strerror(errno));
			continue;
		}
		printf("send %d bytes to client\n", num_bytes);
		usleep(100);

		//
		num_bytes = write(general_fd, controller, strlen(controller));
		if (num_bytes != strlen(controller)) {
			fprintf(stderr, "write controller failed: %s\n",
				strerror(errno));
			continue;
		}
		printf("send %d bytes to client\n", num_bytes);
		usleep(100);
	}

	return NULL;
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
	close(general_fd);
	general_fd = -1;
	printf("Client close socket connection\n");
}

/*
 *
 */
static void
sigint_handler(int sig)
{
	close(general_fd);
	general_fd = -1;
	printf("\nBye ~\n");
	exit(EXIT_SUCCESS);
}
