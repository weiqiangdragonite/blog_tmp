/*
 * server_echo.c
 *
 * Server/Client的通信方式1: 同步短连接
 * 服务器等待客户端的连接，连接客户端后，接收客户端发送过来的数据，
 * 处理后返回给客户端；客户端连接上服务器后，发送数据给服务器，
 * 并等待服务器的返回数据，并显示出来，关闭连接。
 */


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


int
main(int argc, char *argv[])
{
	int sockfd, clifd;
	struct sockaddr_in cli_addr;
	socklen_t cli_len;


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	sockfd = tcp_server();
	if (sockfd == -1) {
		exit(2);
	}

	while (1) {
		cli_len = sizeof(cli_addr);
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr,
				&cli_len);
		if (clifd == -1) {
			perror("accept() failed");
			continue;
		}

		/* 迭代处理 */
		/* 创建子进程来处理 */
		/* 创建子线程来处理 */
		process_request();
	}
}


/*
 * mode == 1: 迭代
 * mode == 2: 子进程
 * mode == 3: 子线程
 */
void
process_request(int mode)
{
	iterator_mode();
}


void
iterator_mode(fd)
{
	str_echo(fd);
	close(fd);
}



void
str_echo(int fd)
{
	ssize_t n;
	char buf[BUF_SIZE];

	while (1) {
		while ((n = read(fd, buf, BUF_SIZE) > 0)
			writen(fd, buf, n);

		if (n < 0 && errno == EINTR)
			continue;
		else if (n < 0)
			perror("str_echo: read() failed");
		else if (n == 0)
			break;	/* EOF */
	}
}






