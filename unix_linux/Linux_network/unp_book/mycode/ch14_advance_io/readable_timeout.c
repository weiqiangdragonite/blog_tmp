

/*
 * 阻塞在select上，等待描述符变成可读，或超时。本函数的返回值
 * 就是select的返回值：出错-1，超时为0，正值为就绪描述符数目。
 * 适用于TCP或UDP
 */
int
readable_timeout(int fd, int sec)
{
	fd_set rset;
	struct timeval tv;

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return (select(fd+1, &rset, NULL, NULL, &tv));
}

