/*
 * File: Server.c
 * Linux下一个socket心跳包的简单实现
 *
 * From: https://blog.csdn.net/freeking101/article/details/78922846
 * 
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


struct CLIENT_ST {
	int client_fd;
	char client_ip[20];
	int heartbeat_count;
};

enum Type { HEART = 1, DATA };
struct PACKET_HEAD
{
	enum Type type;
	int length;
};

struct CLIENT_ST client_data[FD_SETSIZE];
fd_set global_rfds, global_wfds;

static void *detect_heartbeat(void *arg)
{
	int i, fd, count;

	while (1) {
		//printf("Thread: tid = [%lu]\n", pthread_self());
		for (i = 0; i < FD_SETSIZE; i++) { // not good, can use maxi ?
			if (client_data[i].client_fd < 0)
				continue;

			fd = client_data[i].client_fd;
			count = client_data[i].heartbeat_count;
			//printf("Thread: fd = [%d], count = [%d]\n", fd, count);

			// 3s*10没有收到心跳包，判定客户端掉线
			if (count == 10) {
				// 需要用锁吗 ?
				close(fd);
				// 从监听集合中删除此socket连接
				printf("client[%d] offline\n", i);
				FD_CLR(fd, &global_rfds);
				FD_CLR(fd, &global_wfds);
				client_data[i].client_fd = -1;
			} else if (count < 10 && count >= 0) {
				client_data[i].heartbeat_count += 1;
			}
		}

		sleep(3); // 
	}

	return NULL;
}



/*
 * create IPv4 TCP socket for server
 */
int create_socket(void)
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




int main(int argc, char *argv[])
{
	int i, maxi, maxfd, nready;
	fd_set current_rfds, current_wfds;

	//struct client_data[FD_SETSIZE];
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

	/* 初始化select */
	maxfd = socket_fd;
	maxi = -1;	/* index into client[] array */
	memset(client_data, 0, sizeof(struct CLIENT_ST) * FD_SETSIZE);
	for (i = 0; i < FD_SETSIZE; i++)
		client_data[i].client_fd = -1;

	FD_ZERO(&global_rfds);
	FD_ZERO(&global_wfds);

	/* 将监听socket加入select检测的描述符集合 */
	FD_SET(socket_fd, &global_rfds);
	FD_SET(socket_fd, &global_wfds);


	/* 创建心跳检测线程 */
	pthread_t heartbeat_thread;

	if (pthread_create(&heartbeat_thread, NULL, (void *) detect_heartbeat, NULL) != 0) {
		fprintf(stderr, "Create heartbeat_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create heartbeat_thread() success\n");


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
		if (nready < 0) {
			fprintf(stderr, "select() failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// timeout 会每次都清零的
		//printf("timeout after select(): %ld.%03ld\n", (long) timeout.tv_sec, (long) timeout.tv_usec / 1000);
		if (nready == 0) {
			printf("select() timeout\n");
			continue;
		}

		/* new client connection */
		if (FD_ISSET(socket_fd, &current_rfds)) {
			socklen = sizeof(cli_addr);
			connect_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &socklen);
			if (connect_fd < 0) {
				fprintf(stderr, "accept() failed: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			for (i = 0; i < FD_SETSIZE; ++i) {
				/* save connect client fd */
				if (client_data[i].client_fd < 0) {
					printf("Client connected, i = %d\n", i);
					client_data[i].client_fd = connect_fd;
					client_data[i].heartbeat_count = 0;
					/* get client ip */
					memset(client_data[i].client_ip, 0, sizeof(client_data[i].client_ip));
					snprintf(client_data[i].client_ip, sizeof(client_data[i].client_ip), "%s", inet_ntoa(cli_addr.sin_addr));
					printf("new client ip: [%s]\n", client_data[i].client_ip);
					break;
				}
			}

			/* 连接客户不能大于FD_SETSIZE */
			if (i == FD_SETSIZE) {
				fprintf(stderr, "Too many clients\n");
				close(connect_fd);
				continue;
			}

			/* add new client fd to set */
			FD_SET(connect_fd, &global_rfds);
			FD_SET(connect_fd, &global_wfds);

			if (connect_fd > maxfd)
				maxfd = connect_fd;

			if (i > maxi)
				maxi = i;

			/* no more readable descriptor */
			if (--nready <= 0)
				continue;
		}

		/* 监听socket_fd可写? */
		//if (FD_ISSET(socket_fd, &current_wfds)) {
		//	printf("socket_fd writefds have ready\n");
		//	sleep(10);
		//}


		/* check all client fd */
		for (i = 0; i <= maxi; ++i) {
			if ((fd = client_data[i].client_fd) < 0)
				continue;

			memset(buf, 0, sizeof(buf));
			num_bytes = 0;
			struct PACKET_HEAD head;
			memset(&head, 0, sizeof(head));
			int close_conn = 0;
			int nleft = 0;

			// 检测fd是否可读
			if (FD_ISSET(fd, &current_rfds)) {
				// 先接收包头
				num_bytes = read(fd, &head, sizeof(head));

				if (num_bytes <= 0) { // 失败或断开连接
					fprintf(stderr, "read() failed: %s\n", strerror(errno));
					close_conn = 1;
				} else {
					// 最好在包头加多一个识别码，如果是定义的识别码，这是合法包，否则是非法不处理
					printf("recv [%d] bytes:\n", (int) num_bytes);
					printf("packet type = [%s], ", head.type == HEART ? "HEART" : "DATA");
					printf("packet length = [%d]\n", head.length);

					client_data[i].heartbeat_count = 0; // 每次收到心跳包，count置0，data包也需要
					if (head.type == HEART) {
						//client_data[i].heartbeat_count = 0; // 每次收到心跳包，count置0
						printf("receive client[%d] heartbeat\n");
					} else if (head.type == DATA && head.length > 0) {

						printf("packet data = [");
						nleft = head.length;
						while (nleft != 0) { // 需要循环读
							num_bytes = read(fd, buf, nleft);
							if (num_bytes <= 0) { // fail or end
								fprintf(stderr, "read() failed: %s\n", strerror(errno));
								close_conn = 1;
							} else {
								printf("%s", buf);
								nleft -= num_bytes;
							}
						}
						printf("]\n");
					} else {
						printf("other datas\n");
						close_conn = 1;
					}
				}

				if (close_conn) {
					close(fd);
					// 从监听集合中删除此socket连接
					printf("client[%d] close\n", i);
					FD_CLR(fd, &global_rfds);
					FD_CLR(fd, &global_wfds);
					client_data[i].client_fd = -1;
					fd = -1;
				}
			}


			// 检测fd是否可写
			// 这里writefds会一直显示可写(意思是我们可以往fd写数据?)
			if (FD_ISSET(fd, &current_wfds)) {
				printf("fd have ready\n");
				sleep(10);
			}
		}
	}

	close(socket_fd);

	/* wait for return */
	pthread_join(heartbeat_thread, NULL);

	return 0;
}

