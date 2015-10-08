/*
 * Example: select()-based ROT13 server
 *
 * And here’s a reimplementation of our ROT13 server, using select() this time.
 *
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/08
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>


#define PORT 40713
#define BAK_LOG 10
#define MAX_LINE 16384



struct fd_state {
	int fd;
	char buffer[MAX_LINE];
	size_t buffer_used;

	int writing;
	size_t n_written;
	size_t write_upto;
};


struct fd_state *
alloc_fd_state(void)
{
	struct fd_state *state = (struct fd_state *) malloc(sizeof(struct fd_state));
	if (state == NULL) {
		perror("malloc() failed");
		return NULL;
	}

	memset(state, 0, sizeof(struct fd_state));
	state->fd = -1;
	return state;
}

void
free_fd_state(struct fd_state *state)
{
	if (state != NULL)
		free(state);
	state = NULL;
}


void
set_nonblocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}



void
run(void)
{
	int sockfd, clifd, val;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t cli_len;

	/* state is a pointer array */
	struct fd_state *state[FD_SETSIZE];
	int i, maxfd, fd;
	fd_set readset, writeset, errset;
	ssize_t nbytes;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	for (i = 0; i < FD_SETSIZE; ++i)
		state[i] = NULL;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	set_nonblocking(sockfd);

	val = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, BAK_LOG) == -1) {
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}


	maxfd = sockfd;
	while (1) {
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&errset);

		FD_SET(sockfd, &readset);

		for (i = 0; i < FD_SETSIZE; ++i) {
			if (state[i]) {
				FD_SET(state[i]->fd, &readset);

				if (state[i]->fd > maxfd)
					maxfd = state[i]->fd;

				if (state[i]->writing)
					FD_SET(state[i]->fd, &writeset);
			}
		}


		if (select(maxfd + 1, &readset, &writeset, &errset, NULL) == -1) {
			perror("select() failed");
			exit(EXIT_FAILURE);
		}

		/* new client connected */
		if (FD_ISSET(sockfd, &readset)) {
			clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
			if (clifd == -1) {
				perror("accept() failed");
				continue;
			}

			/* I think this way is not good, see the original example code */
			for (i = 0; i < FD_SETSIZE; ++i) {
				/* save connect client fd */
				if (state[i] == NULL) {
					state[i] = alloc_fd_state();
					if (state[i] == NULL)
						exit(EXIT_FAILURE);
						
					set_nonblocking(clifd);
					state[i].fd = clifd;
					break;
				}
			}

			if (i == FD_SETSIZE) {
				fprintf(stderr, "Too many clients\n");
				close(clifd);
			}
		}

		for (i = 0; i < FD_SETSIZE; ++i) {
			fd = state[i]->fd;

			if (FD_ISSET(fd, &readset))
				nbytes = do_read(fd, state[i]);

			if (FD_ISSET(fd, &writeset))
				nbytes == do_write(fd, state[i]);
		}
	}

}


int
main(int argc, char *argv[])
{
	/* set standout unbuffered */
	setvbuf(stdout, NULL, _IONBF, 0);

	run();

	return 0;
}

