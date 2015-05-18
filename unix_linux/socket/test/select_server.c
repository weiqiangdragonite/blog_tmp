/*

参考 UNP

http://www.cnblogs.com/yuxingfirst/archive/2013/03/08/2950281.html
http://blog.csdn.net/boboiask/article/details/4055655
http://blog.csdn.net/sg131971/article/details/6711936

http://blog.csdn.net/hguisu/article/details/7445768


客户端:
1.将打开的socket设为非阻塞的,可以用fcntl(socket, F_SETFL, O_NDELAY)完 
成(有的系统用FNEDLAY也可). 
2.发connect调用,这时返回-1,但是errno被设为EINPROGRESS,意即connect仍旧 
在进行还没有完成. 
3.将打开的socket设进被监视的可写(注意不是可读)文件集合用select进行监视, 
如果可写,用getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, sizeof(int)); 
来得到error的值,如果为零,则connect成功. 

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "inet_socket.h"


#define BUF_SIZE	1024
#define SERVER_PORT	"10000"		/* send/get data from other servers */
#define CLIENT_PORT	"10001"		/* send/get data to clients */


struct client {
	int fd;
	char ip[20];
};


static void *update_data(void *arg);

/*
 * Main function
 */
int
main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);

	/* Create thread */
	pthread_t update_thread;

	/* update_thread */
	if (pthread_create(&update_thread, NULL, (void *) update_data, NULL) != 0) {
		fprintf(stderr, "Create update_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create update_thread() success\n");


	pthread_join(update_thread, NULL);

	return 0;
}




/*
 *
 */
static void *
update_data(void *arg)
{
	char data[] = "Hello, World!\r\n";

	int i, maxi, maxfd, nready, client[FD_SETSIZE];
	fd_set readfds, allfds;
	unsigned char buf[BUF_SIZE];
	ssize_t num_bytes;
	struct timeval timeout;

	struct sockaddr_storage client_addr;
	socklen_t server_addrlen, client_addrlen;
	int socket_fd, connect_fd, fd, is_receive;


	socket_fd = inet_listen(SERVER_PORT, SOMAXCONN, &server_addrlen);
	if (socket_fd == -1) {
		fprintf(stderr, "inet_listen() failed.\n");
		exit(EXIT_FAILURE);
	}

	timeout.tv_sec = 1; /* seconds */
	timeout.tv_usec = 0; /* microseconds */

	// 初始化select
	maxfd = socket_fd;
	maxi = -1;		/* index into client[] array */

	// 
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	FD_ZERO(&allfds);		//清空
	FD_SET(socket_fd, &allfds);	//将监听socket加入select检测的描述符集合

	printf("FD_SETSIZE = %d\n", FD_SETSIZE);


	while (1) {
		readfds = allfds;
		// select也是阻塞式函数，如果不设置超时，那么就会一直阻塞。
		// timeout设置超时时间，但是设置超时要注意，比如超时为1秒，那么第一次
		// 执行select时，如果没有没有准备好，那么等待一秒后会继续向下执行，
		// 到了第2次循环后，select不会继续再等一秒后执行，而是继续向下执行，非阻塞
		//printf("before select()\n");
		nready = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
		//printf("after select()\n");
		//printf("nready = %d\n", nready);

		/* new client connection */
		if (FD_ISSET(socket_fd, &readfds)) {
			client_addrlen = sizeof(struct sockaddr_storage);
			connect_fd = accept(socket_fd,
					(struct sockaddr *) &client_addr,
					&client_addrlen);

			for (i = 0; i < FD_SETSIZE; ++i) {
				/* save connect client fd */
				if (client[i] < 0) {
					printf("Client connected, i = %d\n", i);
					client[i] = connect_fd;
					break;
				}
			}

			/* 连接客户不能大于FD_SETSIZE */
			if (i == FD_SETSIZE)
				fprintf(stderr, "Too many clients\n");

			/* add new client fd to set */
			FD_SET(connect_fd, &allfds);

			if (connect_fd > maxfd)
				maxfd = connect_fd;

			if (i > maxi)
				maxi = i;

			if (--nready <= 0)
				continue;
		}


		/* update data */



		/* check all client */
		for (i = 0; i <= maxi; ++i) {
			fd = client[i];
			if (fd < 0)
				continue;

			memset(buf, 0, sizeof(buf));
			num_bytes = 0;

			// 有客户连接，检测是否有数据
			if (FD_ISSET(fd, &readfds)) {
				num_bytes = read(fd, buf, sizeof(buf));

				if (num_bytes <= 0) {
					close(fd);
					//从监听集合中删除此socket连接
					FD_CLR(fd, &allfds);
					client[i] = -1;
					fd = -1;
				}

			}

			// 有客户连接，发送数据
			if (num_bytes > 0 && fd != -1) {
				num_bytes = write(fd, buf, strlen(buf));

				if (num_bytes <= 0) {
					close(fd);
					//从监听集合中删除此socket连接
					FD_CLR(fd, &allfds);
					client[i] = -1;
					fd = -1;
				}
			}
		}

		//sleep(1);
	}

	close(socket_fd);

}








