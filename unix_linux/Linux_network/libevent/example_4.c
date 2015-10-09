/*
 * Example: A low-level ROT13 server with Libevent
 *
 * Here’s yet another version of our asynchronous ROT13 server. This time,
 * it uses Libevent 2 instead of select().
 *
 * Original example from Libevent Documentation Book: Programming with Libevent.
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/09
 *
 * Compile: gcc -Wall xxx.c -levent
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <event2/event.h>


#define PORT 40713
#define BAK_LOG 10
#define MAX_LINE 16384

struct fd_state {
	char buffer[MAX_LINE];
	size_t buffer_used;

	size_t n_written;
	size_t write_upto;

	struct event *read_event;
	struct event *write_event;
};



void do_read(evutil_socket_t fd, short events, void *arg);
void do_write(evutil_socket_t fd, short events, void *arg);
struct fd_state *alloc_fd_state(struct event_base *base, evutil_socket_t fd);
void free_fd_state(struct fd_state *state);





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


void
do_read(evutil_socket_t fd, short events, void *arg)
{
	struct fd_state *state = arg;
	char buf[1024];
	int i;
	ssize_t result;

	while (1) {
		assert(state->write_event);
		result = read(fd, buf, sizeof(buf));
		if (result <= 0)
			break;

		for (i = 0; i < result; ++i) {
			if (state->buffer_used < sizeof(state->buffer))
				state->buffer[state->buffer_used++] = rot13_char(buf[i]);
			if (buf[i] == '\n') {
				assert(state->write_event);
				event_add(state->write_event, NULL);
				state->write_upto = state->buffer_used;
			}
		}
	}

	if (result == 0) {
		free_fd_state(state);
	} else if (result < 0) {
		if (errno == EAGAIN)
			return;
		perror("read() failed");
		free_fd_state(state);
	}
}


void
do_write(evutil_socket_t fd, short events, void *arg)
{
	struct fd_state *state = arg;
	ssize_t result;

	while (state->n_written < state->write_upto) {
		result = write(fd, state->buffer + state->n_written,
				state->write_upto - state->n_written);

		if (result < 0) {
			if (errno == EAGAIN)
				return;
			perror("write() failed");
			free_fd_state(state);
			return;
		}

		assert(result != 0);

		state->n_written += result;
	}

	if (state->n_written == state->buffer_used)
		state->n_written = state->write_upto = state->buffer_used = 0;

	event_del(state->write_event);
}




struct fd_state *
alloc_fd_state(struct event_base *base, evutil_socket_t fd)
{
	struct fd_state *state = malloc(sizeof(struct fd_state));
	if (state == NULL) {
		perror("malloc() failed");
		return state;
	}

	state->read_event = event_new(base, fd, EV_READ | EV_PERSIST, do_read, state);
	if (state->read_event == NULL) {
		free(state);
		return NULL;
	}

	state->write_event = event_new(base, fd, EV_WRITE | EV_PERSIST, do_write, state);
	if (state->write_event == NULL) {
		event_free(state->read_event);
		free(state);
		return NULL;
	}

	state->buffer_used = state->n_written = state->write_upto = 0;


	assert(state->write_event);
	return state;
}

void
free_fd_state(struct fd_state *state)
{
	event_free(state->read_event);
	event_free(state->write_event);
	free(state);
}





void
do_accept(evutil_socket_t sockfd, short event, void *arg)
{
	struct event_base *base = arg;
	struct sockaddr_in cli_addr;
	struct fd_state *state;
	socklen_t len;
	int clifd;

	len = sizeof(cli_addr);
	clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &len);

	if (clifd < 0) {
		perror("accept() failed");
	} else if (clifd > FD_SETSIZE) {
		close(clifd);
	} else {
		evutil_make_socket_nonblocking(clifd);
		state = alloc_fd_state(base, clifd);

		assert(state);
		assert(state->write_event);

		event_add(state->read_event, NULL);
	}
}


void
run(void)
{
	evutil_socket_t sockfd;
	struct sockaddr_in serv_addr;
	struct event_base *base;
	struct event *listener_event;

	int val;


	base = event_base_new();
	if (base == NULL) {
		fprintf(stderr, "event_base_new() failed\n");
		exit(EXIT_FAILURE);
	}


	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}
	evutil_make_socket_nonblocking(sockfd);

	val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
		perror("setsockopt() failed");
		exit(EXIT_FAILURE);
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, BAK_LOG) == -1) {
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}


	listener_event = event_new(base, sockfd, EV_READ | EV_PERSIST, do_accept, (void *) base);
	event_add(listener_event, NULL);

	event_base_dispatch(base);
}



int
main(int argc, char *argv[])
{
	/* set no buffered */
	setvbuf(stdout, NULL, _IONBF, 0);

	run();
	return 0;
}






