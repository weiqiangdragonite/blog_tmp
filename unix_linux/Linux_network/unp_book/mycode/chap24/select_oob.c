/*
 * 用select接收带外数据
 */


int
main(int argc, char *argv[])
{
	int listenfd, connfd, n;
	int justreadoob = 0;
	char buf[128];
	fd_set rset, eset;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], NULL);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], NULL);
	else {
		fprintf(stderr, "Usage: %s [<host>] <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	connfd = accept(listenfd, NULL, NULL);

	FD_ZERO(&rset);
	FD_ZERO(&eset);

	while (1) {
		FD_SET(connfd, &rset);
		if (justreadoob == 0)
			FD_SET(connfd, &eset);

		select(connfd + 1, &rset, NULL, &eset, NULL);

		if (FD_ISSET(connfd, &eset)) {
			n = recv(connfd, buf, sizeof(buf) - 1, MSG_OOB);
			buf[n] = '\0';
			printf("read %d OOB byte: %s\n", buf);
			justreadoob = 1;
			FD_CLR(connfd, &eset);
		}

		if (FD_ISSET(connfd, &rset)) {
			if ((n = read(connfd, buf, sizeof(buf) - 1)) == 0) {
				printf("receive EOF\n");
				exit(EXIT_SUCCESS);
			}
			buf[n] = '\0';
			printf("read %d bytes: %s\n", buf);
			justreadoob = 0;
		}
	}
}



