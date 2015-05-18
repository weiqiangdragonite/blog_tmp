/*
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "inet_socket.h"


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define MAIN_SERVER	"192.168.1.130"
#define SERVER_PORT	"10000"


/* function prototypes */
static void *server_thread(void *arg);
static void *door_thread(void *arg);
static void *data_thread(void *arg);


int
main(int argc, char *argv[])
{
	// get info from mysql
	//get_info_sql();

	// create thread
	pthread_t connect_server_thread;
	pthread_t connect_door_thread;
	pthread_t update_data_thread;
	
	if (pthread_create(&connect_server_thread, NULL, (void *) server_thread, NULL) != 0) {
		fprintf(stderr, "Create connect_server_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&connect_door_thread, NULL, (void *) door_thread, NULL) != 0) {
		fprintf(stderr, "Create connect_door_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&update_data_thread, NULL, (void *) data_thread, NULL) != 0) {
		fprintf(stderr, "Create update_data_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	pthread_join(connect_server_thread, NULL);
	pthread_join(connect_door_thread, NULL);
	pthread_join(update_data_thread, NULL);
	

	return 0;
}

static void *
server_thread(void *arg)
{
	return NULL;
	int server_fd, maxfd, nready;
	fd_set readfds, writefds, allfds;
	struct timeval timeout;
	char buf[BUF_SIZE];
	ssize_t num_bytes;
	int connected;

	timeout.tv_sec = 1;	/* seconds */
	timeout.tv_usec = 0;	/* microseconds */

	while (1) {
		server_fd = -1;
		connected = 0;

		server_fd = inet_connect(MAIN_SERVER, SERVER_PORT, SOCK_STREAM, 1);
		printf("server_fd = %d\n", server_fd);

		if (server_fd == -1) {
			fprintf(stderr, "inet_connect() failed: Cannot connect to main server\n");
			sleep(1);
			continue;
		}

		// 初始化select
		maxfd = server_fd;
		FD_ZERO(&allfds);		//清空
		FD_SET(server_fd, &allfds);	//将监听socket加入select检测的描述符集合


		// 应该是监听writefds，直到连接上
		while (1) {
			writefds = allfds;
			nready = select(maxfd + 1, NULL, &writefds, NULL, &timeout);
			printf("nready = %d\n", nready);

			// timeout
			if (nready == 0) {
				fprintf(stderr, "select() timeout\n");
				sleep(1);
				continue;
			// error
			} else if (nready == -1) {
				fprintf(stderr, "select() failed: %s\n", strerror(errno));
				sleep(1);
				continue;
			}

			// nready = 1
			if (FD_ISSET(server_fd, &writefds)) {
				int error;
				socklen_t len = sizeof(error);
				int res;
				res = getsockopt(server_fd, SOL_SOCKET, SO_ERROR, &error, &len);
				if (res == -1) {
					fprintf(stderr, "getsockopt() readfds failed: %s\n", strerror(errno));
					sleep(1);
					continue;
				}
				printf("Connected to server.\n");
				connected = 1;
				break;
			}

			sleep(1);
		}


		// 监听socket
		while (connected) {
			//
			readfds = allfds;
			nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
			printf("nready = %d\n", nready);

			// timeout
			if (nready == 0) {
				fprintf(stderr, "select() timeout\n");
				sleep(1);
				continue;
			// error
			} else if (nready == -1) {
				fprintf(stderr, "select() failed: %s\n", strerror(errno));
				sleep(1);
				continue;
			}

			// nready = 1
			if (FD_ISSET(server_fd, &writefds)) {
				num_bytes = readline(server_fd, buf, sizeof(buf));
				printf("recv %ld bytes\n", num_bytes);

				if (num_bytes <= 0)
					break;
			}
		}

		close(server_fd);
		fprintf(stderr, "Close from main server\n");
		sleep(1);
	}

	return NULL;
}

static void *
door_thread(void *arg)
{
	int door_fd;
	char buf[BUF_SIZE];
	ssize_t num_bytes;

	while (1) {
		door_fd = -1;
		door_fd = inet_connect(MAIN_SERVER, "8000", SOCK_STREAM, 0);
		if (door_fd == -1) {
			fprintf(stderr, "inet_connect() failed: Cannot connect to door server\n");
			sleep(1);
			continue;
		}
		printf("Connected to door server\n");

		while (1) {
			num_bytes = read(door_fd, buf, sizeof(buf));
			if (num_bytes <= 0) {
				fprintf(stderr, "door server had closed\n");
				break;
			}

			printf("recv %ld bytes\n", num_bytes);
		}
		close(door_fd);
		sleep(1);
	}

	return NULL;
}

static void *
data_thread(void *arg)
{
	while (1) {
		sleep(1);
	}

	return NULL;
}






























