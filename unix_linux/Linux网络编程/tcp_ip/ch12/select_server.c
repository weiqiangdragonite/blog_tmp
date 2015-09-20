/*

参考 UNP

http://www.cnblogs.com/yuxingfirst/archive/2013/03/08/2950281.html
http://blog.csdn.net/boboiask/article/details/4055655
http://blog.csdn.net/sg131971/article/details/6711936

http://blog.csdn.net/hguisu/article/details/7445768



1.将打开的socket设为非阻塞的,可以用fcntl(socket, F_SETFL, O_NONBLOCK)完 
成. 
2.发connect调用,这时返回-1,但是errno被设为EINPROGRESS,意即connect仍旧 
在进行还没有完成. 


*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>


#define BUF_SIZE	1024
#define BACK_LOG	128
#define SERV_PORT	10000



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
 * create IPv4 TCP socket for server
 */
int
create_socket(void)
{
	struct sockaddr_in svaddr;
	int fd, opt;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		return -1;
	}

	opt = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
		fprintf(stderr, "setsockopt() failed: %s\n", strerror(errno));  
		return -1;
	}

	memset(&svaddr, 0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svaddr.sin_port = htons(SERV_PORT);


	if (bind(fd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
		fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	if (listen(fd, BACK_LOG) == -1) {
		fprintf(stderr, "listen() failed: %s\n", strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}



/*
 *
 */
static void *
update_data(void *arg)
{

	int i, maxi, maxfd, nready;
	fd_set global_rfds, global_wfds, current_rfds, current_wfds;

	int client[FD_SETSIZE];
	char buf[BUF_SIZE];
	ssize_t num_bytes;
	struct timeval timeout;

	struct sockaddr_in cli_addr;
	socklen_t socklen;
	int socket_fd, connect_fd, fd;




	socket_fd = create_socket();
	if (socket_fd == -1) {
		fprintf(stderr, "create_socket() failed.\n");
		exit(EXIT_FAILURE);
	}

	// 初始化select
	maxfd = socket_fd;
	maxi = -1;		/* index into client[] array */

	// 
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	FD_ZERO(&global_rfds);		//清空
	FD_ZERO(&global_wfds);

	FD_SET(socket_fd, &global_rfds);	//将监听socket加入select检测的描述符集合
	FD_SET(socket_fd, &global_wfds);

	printf("FD_SETSIZE = %d\n", FD_SETSIZE);



	socklen = sizeof(cli_addr);
	while (1) {

		// 等待5.5秒
		timeout.tv_sec = 5;	/* seconds */
		timeout.tv_usec = 500 * 1000;	/* microseconds 微秒 */

		current_rfds = global_rfds;
		current_wfds = global_wfds;

		// select也是阻塞式函数，如果不设置超时，那么就会一直阻塞。

		// timeout设置超时时间，但是设置超时要注意，比如超时为1秒，那么第一次
		// 执行select时，如果没有没有准备好，那么等待一秒后会继续向下执行，
		// 到了第2次循环后，select不会继续再等一秒后执行，而是继续向下执行，非阻塞
		// 所以要在前面重新设置timeout


		nready = select(maxfd + 1, &current_rfds, &current_wfds, NULL, &timeout);
		//printf("after select()\n");
		//printf("nready = %d\n", nready);

		//printf("timeout after select(): %ld.%03ld\n", (long) timeout.tv_sec, (long) timeout.tv_usec / 1000);

		/* new client connection */
		if (FD_ISSET(socket_fd, &current_rfds)) {
			connect_fd = accept(socket_fd,
					(struct sockaddr *) &cli_addr,
					&socklen);

			for (i = 0; i < FD_SETSIZE; ++i) {
				/* save connect client fd */
				if (client[i] < 0) {
					printf("Client connected, i = %d\n", i);
					client[i] = connect_fd;
					break;
				}
			}

			/* 连接客户不能大于FD_SETSIZE */
			if (i == FD_SETSIZE) {
				fprintf(stderr, "Too many clients\n");
				//exit(EXIT_FAILURE);
			}

			/* add new client fd to set */
			FD_SET(connect_fd, &global_rfds);
			FD_SET(connect_fd, &global_wfds);

			if (connect_fd > maxfd)
				maxfd = connect_fd;

			if (i > maxi)
				maxi = i;

			// no more readable descriptor
			//if (--nready <= 0)
			//	continue;
		}


		if (FD_ISSET(socket_fd, &current_wfds)) {
			printf("writefds have ready\n");
			sleep(10);
		}

		// no more readable descriptor
		//if (--nready <= 0)
		//	continue;


		/* update data */



		/* check all client fd */
		for (i = 0; i <= maxi; ++i) {
			if ((fd = client[i]) < 0)
				continue;

			memset(buf, 0, sizeof(buf));
			num_bytes = 0;

			// 检测fd是否可读
			if (FD_ISSET(fd, &current_rfds)) {
				num_bytes = read(fd, buf, sizeof(buf));

				if (num_bytes <= 0) {
					close(fd);
					// 从监听集合中删除此socket连接
					printf("client[%d] close\n", i);
					FD_CLR(fd, &global_rfds);
					FD_CLR(fd, &global_wfds);
					client[i] = -1;
					fd = -1;
				} else
					printf("recv [%d bytes]: %s\n", (int) num_bytes, buf);
			}

			// 检测fd是否可写
			// 如果客户端发了消息给服务器端，然后这里writefds会一直显示有数据
			if (FD_ISSET(fd, &current_wfds)) {
				printf("fd have ready\n");
				sleep(10);
			}
		}
	}

	close(socket_fd);
}








