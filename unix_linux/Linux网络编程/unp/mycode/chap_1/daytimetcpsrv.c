/*
 * 一、启动时间服务（time服务的知名端口是13）
 * 1、发现xinetd服务还未安装，可以用yum install xinetd安装
 * 2、vim编辑/etc/xinetd.d/daytime-stream文件，将disable = yes改为disable = no
 * 3、注销系统或重启xinetd服务，用service xinetd stop然后service xinetd start
 * 4、查看网络状态netstat -ant，即可看到
 * tcp        0      0 0.0.0.0:13              0.0.0.0:*               LISTEN
 * 这是daytime服务的网络监听端口状态
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define BACK_LOG	10

int
main(int argc, char *argv[])
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buf[128];
	time_t ticks;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed: ");
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);	/* daytime server */


	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	if (listen(listenfd, BACK_LOG) == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		close(listenfd);
		return -1;
	}

	while (1) {
		connfd = accept(listenfd, NULL, NULL);
		if (connfd == -1) {
			fprintf(stderr, "accept() failed: %s\n", strerror(errno));
			continue;
		}

		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));

		if (write(connfd, buf, strlen(buf)) != strlen(buf))
			fprintf(stderr, "write() failed: %s\n", strerror(errno));
		else
			printf("write %ld: %s\n", strlen(buf), buf);
		close(connfd);
	}
	close(listenfd);

	return 0;
}


