/*
 * Example: A simpler ROT13 server with Libevent
 *
 * Here’s our ROT13 server one last time, using the bufferevents API.
 *
 * Original example from Libevent Documentation Book: Programming with Libevent.
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/010
 *
 * Compile: gcc -Wall xxx.c -levent_core
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>



#define PORT 40713
#define BAK_LOG 10
#define MAX_LINE 16384



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
readcb(struct bufferevent *bev, void *arg)
{
	struct evbuffer *input, *output;
	char *line;
	size_t n;
	int i;

	input = bufferevent_get_input(bev);
	output = bufferevent_get_output(bev);

	while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
		for (i = 0; i < n; ++i)
			line[i] = rot13_char(line[i]);
		evbuffer_add(output, line, n);
		evbuffer_add(output, "\n", 1);
		free(line);
	}

	/* I don't know what this mean */
	if (evbuffer_get_length(input) >= MAX_LINE) {
		/* Too long; just process what there is and go on so that
		 * the buffer doesn't grow infinitely long. */
		char buf[1024];
		while (evbuffer_get_length(input)) {
			int n = evbuffer_remove(input, buf, sizeof(buf));
			for (i = 0; i < n; ++i)
				buf[i] = rot13_char(buf[i]);

			evbuffer_add(output, buf, n);
		}
		evbuffer_add(output, "\n", 1);
	}
}


void
errorcb(struct bufferevent *bev, short error, void *arg)
{
	if (error & BEV_EVENT_EOF) {
		/* connecton has been closed */
		/* TODO */
	} else if (error & BEV_EVENT_ERROR) {
		/* check errno to see what error occured */
		/* TODO */
	} else if (error & BEV_EVENT_TIMEOUT) {
		/* timeout handle */
		/* TODO */
	}

	bufferevent_free(bev);
}


void
do_accept(evutil_socket_t sockfd, short event, void *arg)
{
	struct event_base *base = arg;
	struct sockaddr_in cli_addr;
	socklen_t len = sizeof(cli_addr);
	int fd;

	fd = accept(sockfd, (struct sockaddr *) &cli_addr, &len);
	if (fd == -1) {
		perror("accept() failed");
	} else if (fd > FD_SETSIZE) {
		fprintf(stderr, "Too many clients\n");
		close(fd);
	} else {
		struct bufferevent *bev;
		evutil_make_socket_nonblocking(fd);
		bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
		bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
		bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
		bufferevent_enable(bev, EV_READ | EV_WRITE);
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
		fprintf(stderr, "event_base_new() failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	evutil_make_socket_nonblocking(sockfd);


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

	listener_event = event_new(base, sockfd, EV_READ | EV_PERSIST, do_accept, (void *) base);
	event_add(listener_event, NULL);

	event_base_dispatch(base);
}




int
main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	run();
	return 0;
}


