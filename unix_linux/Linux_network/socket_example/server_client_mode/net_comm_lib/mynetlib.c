/*
 *
 */


/*
 * write n bytes to fd
 */
ssize_t
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

/*
 * read n bytes from fd
 */
ssize_t
readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0)
			break;	/* break */

		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);
}




/*
 * for TCP server v1
 * create tcp socket, bind to server, listen for request
 * success return socket fd, otherwise return -1
 */
int
tcp_listen_v1(const int port)
{
	int fd;
	struct sockaddr_in serv_addr;

	/* create socket */
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket() failed");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INARRR_ANY);
	serv_addr.sin_port = htons(port);

	/* bind to address */
	if (bind(fd, (struct sockaddr *) &serv_addr,
	sizeof(serv_addr)) == -1) {
		perror("bind() failed");
		close(fd);
		return -1;
	}

#ifndef BACK_LOG
#define BACK_LOG 128
#endif
	/* set listen queue */
	if (listen(fd, BACK_LOG) == -1) {
		perror("listen() failed");
		close(fd);
		return -1;
	}

	return fd;
}







