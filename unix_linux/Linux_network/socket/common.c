/*
 * Socket write/read pack function from UNP and other examples.
 *
 * rewrite by weiqiangdragonite@gmail.com
 * updated on 2015/10/10
 */


/*
 * read n bytes from fd
 */
ssize_t
readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	size_t nread;
	char *p;

	p = (char *) vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, p, nleft)) < 0) {
			if (errno == EINTR)	/* interrupt by system */
				nread = 0;	/* and call read() again */
			else
				return -1;
		} else if (nread == 0) {
			break;	/*EOF */
		}

		nleft -= nread;
		p += nread;
	}

	return (n - nleft);	/* return n >= 0 */
}



/*
 * write n bytes to fd
 */
ssize_t
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *p;

	p = (const char *) vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, p, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;	/* call write() again */
			else
				return -1;
		}

		nleft -= nwritten;
		p += nwritten;
	}

	return n;
}


/*
 *
 */
ssize_t
readline(int fd, void *ptr, size_t maxlen)
{
	sszie_t n, num_read;
	char c;
	char *p;

	p = ptr;
	for (n = 1; n < maxlen; ++n) {
		num_read = read(fd, &c, 1);

		if (num_read == -1) {
			if (errno == EINTR)	/* interrupt, restart */
				continue;
			else
				return -1;
		} else if (num_read == 0) {
			*p = '\0';		/* EOF, (n - 1) bytes return */
			return (n - 1);
		} else {
			*p++ = c;		/* read to maxlen - 1 bytes */
			if (c == '\n')
				break;
		}
	}

	*p = '\0';
	return n;
}


