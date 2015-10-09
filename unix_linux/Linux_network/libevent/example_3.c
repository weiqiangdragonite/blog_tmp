/*
 * Example: select()-based ROT13 server
 *
 * And here’s a reimplementation of our ROT13 server, using select() this time.
 *
 * Original example from Libevent Documentation Book: Programming with Libevent.
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/09
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
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




char
rot13_char(char c)
{
	if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
		return c + 13;
	else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
		return c - 13;
	else
		return c;
}


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
	//state = NULL;
}


void
set_nonblocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}


int
do_read(int fd, struct fd_state *state)
{
	char buf[1024];
	int i;
	ssize_t result;

	//printf("fd = %d in do_read()\n", fd);

	while (1) {
		result = read(fd, buf, sizeof(buf));
		if (result <= 0)
			break;

		for (i = 0; i < result; ++i) {
			if (state->buffer_used < sizeof(state->buffer))
				state->buffer[state->buffer_used++] = rot13_char(buf[i]);
			if (buf[i] == '\n') {
				state->writing = 1;
				state->write_upto = state->buffer_used;
			}
		}
	}

	if (result == 0) {
		return 1;
	} else if (result < 0) {
		if (errno == EAGAIN) {
			return 0;
		} else {
			perror("read() failed");
			return -1;
		}
	}

	return 0;
}


int
do_write(int fd, struct fd_state *state)
{
	ssize_t nbytes;

	//printf("fd = %d in do_write()\n", fd);

	while (state->n_written < state->write_upto) {
		nbytes = write(fd, state->buffer + state->n_written,
			state->write_upto - state->n_written);

		if (nbytes < 0) {
			if (errno == EAGAIN)
				return 0;
			return -1;
		}

		state->n_written += nbytes;
	}

	if (state->n_written == state->buffer_used) {
		state->n_written = state->write_upto = state->buffer_used = 0;
		state->writing = 0;
	}

	return 0;
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
	ssize_t result;

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
					printf("conneced fd = %d\n", clifd);
					set_nonblocking(clifd);
					state[i]->fd = clifd;
					break;
				}
			}

			if (i == FD_SETSIZE) {
				fprintf(stderr, "Too many clients\n");
				close(clifd);
			}
		}


		/* recv and send datas */
		for (i = 0; i < FD_SETSIZE; ++i) {
			if (state[i] == NULL)
				continue;

			result = 0;
			fd = state[i]->fd;

			if (FD_ISSET(fd, &readset)) {
				result = do_read(fd, state[i]);
				//printf("fd = %d, do_read() return %d\n", fd, (int) result);
			}

			if (result == 0 && FD_ISSET(fd, &writeset)) {
				result = do_write(fd, state[i]);
				//printf("fd = %d, do_write() return %d\n", fd, (int) result);
			}

			/* result == 1 || result == -1 -> client closed or error */
			if (result) {
				printf("closed fd = %d\n", fd);
				close(fd);
				free_fd_state(state[i]);
				state[i] = NULL;
			}
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

