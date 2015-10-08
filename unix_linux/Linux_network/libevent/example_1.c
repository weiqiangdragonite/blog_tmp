/*
 * Example: A simple blocking HTTP client
 *
 * Here’s an example of a really simple client using blocking network calls.
 * It opens a connection to www.google.com, sends it a simple HTTP request,
 * and prints the response to stdout.
 *
 *
 * rewrite by <weiqiangdragonite@gmail.com>
 * update on 2015/10/08
 *
 * I think the original example has some problems, so I rewrite it on my way.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

int
main(int argc, char *argv[])
{
	const char query[] = 
		"GET / HTTP/1.0\r\n"
		"Host: www.baidu.com\r\n"
		"\r\n";
	const char hostname[] = "www.baidu.com";
	struct sockaddr_in serv_addr;
	struct hostent *host;
	const char *cp;
	int fd, i;
	ssize_t n_bytes, remaining;
	char buf[1024];

	/* Look uo the IP address for the hostname, this isn't threadsafe */
	host = gethostbyname(hostname);
	if (host == NULL) {
		fprintf(stderr, "Couldn't lookup %s: %s\n",
			hostname, hstrerror(h_errno));
		exit(EXIT_FAILURE);
	}
	if (host->h_addrtype != AF_INET) {
		fprintf(stderr, "No ipv6 support, sorry\n");
		exit(EXIT_FAILURE);
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	/* connect to remote host */
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(80);
	for (i = 0; host->h_addr_list[i]; ++i) {
		serv_addr.sin_addr = *(struct in_addr *) host->h_addr_list[i];
		break;
	}

	if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		perror("connect() failed");
		close(fd);
		exit(EXIT_FAILURE);
	}

	/* write the query */
	cp = query;
	remaining = strlen(query);
	while (remaining) {
		n_bytes = write(fd, cp, remaining);
		if (n_bytes <= 0) {
			perror("write() failed");
			exit(EXIT_FAILURE);
		}
		remaining -= n_bytes;
		cp += n_bytes;
	}

	/* get answer back */
	while (1) {
		n_bytes = read(fd, buf, sizeof(buf));
		if (n_bytes == 0) {
			break;
		} else if (n_bytes < 0) {
			perror("read() failed");
			close(fd);
			exit(EXIT_FAILURE);
		}

		fwrite(buf, n_bytes, 1, stdout);
	}

	close(fd);

	return 0;
}


