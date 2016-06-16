/*
 * Socket write/read pack function from UNP and other examples.
 *
 * rewrite by weiqiangdragonite@gmail.com
 * updated on 2015/10/10
 */


/*
 * read n bytes from fd
 * no buffer
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
 * no buffer
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
 * this code is combine tlpi and unp
 *
 * PAINFULLY SLOW VERSION !!!
 */
ssize_t
readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t n, num_read;
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


/******************************************************************************/
/* below is from csapp, provide buffer function */

#define RIO_BUFSIZE 8192

struct rio {
	int rio_fd;		/* fd for this internal buf */
	int rio_cnt;		/* unread bytes in internal buf */
	char *rio_bufptr;	/* next unread bytes */
	char rio_buf[RIO_BUFSIZE];
};

typedef struct rio rio_t;

/*
 * you should call rio_readinitb() first to use rio_xxx() function
 */
void
rio_readinitb(rio_t *rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}


/*
 * if buffer is empty, first call read() to fill in buffer, then copy
 * n bytes data to user buf
 */
static ssize_t
rio_read(rio_t *rp, char *usrbuf, size_t n)
{
	int cnt;

	while (rp->rio_cnt <= 0) {
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf,
				sizeof(rp->rio_buf));
		if (rp->rio_cnt < 0) {
			if (error != EINTR)
				return -1;
		} else if (rp->rio_cnt == 0) {
			return 0;	/* EOF */
		} else {
			rp->rio_bufptr = rp->rio_buf;	/* reset buf ptr */
		}
	}

	/* copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
	cnt = n;
	if (rp->rio_cnt < n)
		cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;

	return cnt;
}



/*
 * read one line, read most maxlen-1 bytes into usrbuf
 * with buffer
 */
ssize_t
rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
	int n, rc;
	char c;
	char *bufp = (char *) usrbuf;

	for (n = 1; n < maxlen; ++n) {
		if ((rc = rio_read(rp, &c, 1)) == 1) {
			*bufp++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {		/* EOF */
			if (n == 1)
				return 0;	/* no data read */
			else
				break;		/* some datas read */
		} else {
			return -1;	/* error */
		}
	}

	*bufp = '\0';
	return n;
}

/*
 * read n bytes data
 * with buffer
 */
ssize_t
rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;

	while (nleft > 0) {
		if ((nread = rio_read(rp, bufp, nleft)) < 0) {
			if (errno == EINTR)	/* interrupt by system */
				nread = 0;	/* and call read() again */
			else
				return -1;
		} else if (nread == 0) {
			break;	/*EOF */
		}

		nleft -= nread;
		bufp += nread;
	}

	return (n - nleft);	/* return >= 0 */
}






