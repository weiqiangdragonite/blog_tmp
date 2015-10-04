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

#include <arpa/inet.h>
#include <sys/socket.h>


int
main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	ssize_t n;
	char buf[128];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
		return -1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket() failed: ");
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) != 1) {
		fprintf(stderr, "inet_pton() for %s failed: %s\n",
			argv[1], strerror(errno));
		close(sockfd);
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		fprintf(stderr, "connect() failed: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}


	while ((n = read(sockfd, buf, sizeof(buf))) > 0) {
		buf[n] = '\0';
		if (fputs(buf, stdout) == EOF) {
			fprintf(stderr, "fputs() error\n");
			close(sockfd);
			return -1;
		}
	}
	if (n < 0) {
		fprintf(stderr, "read() failed: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	close(sockfd);

	return 0;
}


