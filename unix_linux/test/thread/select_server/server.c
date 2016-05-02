/*

参考 UNP

http://www.cnblogs.com/yuxingfirst/archive/2013/03/08/2950281.html
http://blog.csdn.net/boboiask/article/details/4055655
http://blog.csdn.net/sg131971/article/details/6711936

http://blog.csdn.net/hguisu/article/details/7445768

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


struct node {
	int fd;
	unsigned char type;
	unsigned char data[BUF_SZIE];
	struct node *next;
};

struct device_queue {
	struct node *front;
	struct node *rear;
};

struct queue {
	struct node *front;
	struct node *rear;
};

typedef struct queue queue_t;

queue_t *device_queue;

insert_queue(queue_t queue, struct node item)
{
	struct node *ptr = (struct node *) malloc(sizeof(struct node));
	node->data = item;
	node->next = NULL;

	/* 在对尾插入新的元素 */
	queue->rear->next = ptr;
	/* 对尾指向新元素 */
	queue->read = ptr;
}




struct client {
	int fd;
	char ip[20];
	unsigned char data[1024];
};

typedef struct client client_t;


struct server {
	int fd;
	char ip[20];
};



struct server_node {
	char server_ip[20];
	int server_type;
	void *dev_list;
	struct server_node *next;
};
typedef struct server_node server_t;
struct server_node *server_list = NULL;




static void *update_data(void *arg);

/*
 * Main function
 */
int
main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);


	// get server info from mysql
	// SELECT XXX FROM server_table
	server_t *newptr = NULL;
	newptr = (server_t *) calloc(1, sizeof(server_t));
	if (newptr == NULL) {
		fprintf(stderr, "calloc() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	snprintf();


	// get info from mysql
	is_main_server = 1;




	/* Create thread */
	pthread_t update_thread;
	pthread_t connect_thread;

	/* update_thread */
	if (pthread_create(&update_thread, NULL, (void *) update_data, NULL) != 0) {
		fprintf(stderr, "Create update_thread() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Create update_thread() success\n");


	pthread_join(update_thread, NULL);

	return 0;
}


client_t client[FD_SETSIZE];
server_t server[FD_SETSIZE];

static void *
connect_func(void *arg)
{
	int i;
	int server_nready, client_nready;
	int server_allfds, client_allfds, server_nfds, client_nfds;
	int server_readfds, client_readfds, server_num, client_num;
	int connect_fd;
	fd_set readfds, allfds;
	unsigned char buf[BUF_SIZE];
	ssize_t num_bytes;
	struct timeval timeout;

	struct sockaddr_storage client_addr;
	socklen_t server_addrlen, client_addrlen;
	int connect_server_fd, connect_client_fd;


	connect_server_fd = inet_listen(SERVER_PORT, SOMAXCONN, &server_addrlen);
	if (connect_server_fd == -1) {
		fprintf(stderr, "inet_listen() failed.\n");
		exit(EXIT_FAILURE);
	}

	connect_client_fd = inet_listen(CLIENT_PORT, SOMAXCONN, &server_addrlen);
	if (connect_client_fd == -1) {
		fprintf(stderr, "inet_listen() failed.\n");
		exit(EXIT_FAILURE);
	}


	timeout.tv_sec = 1; /* seconds */
	timeout.tv_usec = 0; /* microseconds */


	/* init select */
	server_nfds = connect_server_fd;
	client_nfds = connect_client_fd;
	/* server[] / client[] array */
	server_num = client_num = -1;


	memset(client, 0, sizeof(server));
	memset(client, 0, sizeof(client));
	for (i = 0; i < FD_SETSIZE; i++)
		client[i].fd = server[i].fd = -1;


	FD_ZERO(&server_allfds);
	FD_ZERO(&client_allfds);
	/* 将监听socket加入select检测的描述符集合 */
	FD_SET(connect_server_fd, &allfds);
	FD_SET(connect_client_fd, &allfds);

	//printf("FD_SETSIZE = %d\n", FD_SETSIZE);


	while (1) {
		server_readfds = server_allfds;
		client_readfds = client_allfds;

		// select也是阻塞式函数，如果不设置超时，那么就会一直阻塞。
		// timeout设置超时时间，但是设置超时要注意，比如超时为1秒，那么第一次
		// 执行select时，如果没有没有准备好，那么等待一秒后会继续向下执行，
		// 到了第2次循环后，select不会继续再等一秒后执行，而是继续向下执行，非阻塞
		server_nready = select(server_nfds + 1, &server_readfds, NULL, NULL, &timeout);
		client_nready = select(client_nfds + 1, &client_readfds, NULL, NULL, &timeout);


		/* new server connection */
		if (FD_ISSET(connect_server_fd, &server_readfds)) {
			client_addrlen = sizeof(struct sockaddr_storage);
			connect_fd = accept(connect_server_fd, (struct sockaddr *) &client_addr, &client_addrlen);

			for (i = 0; i < FD_SETSIZE; ++i) {
				/* save connect client fd */
				if (server[i].fd != -1)
					continue;

				server[i].fd = connect_fd;
				snprintf(server[i].ip, sizeof(server[i].ip), "%s",
					inet_addrstr((struct sockaddr *) &client_addr, client_addrlen));
				printf("Server(%d) connected, ip = %s\n", i, server[i].ip);
				break;
			}

			/* 连接客户不能大于FD_SETSIZE */
			if (i == FD_SETSIZE)
				fprintf(stderr, "Too many servers\n");

			/* add new server fd to set */
			FD_SET(connect_fd, &server_allfds);

			if (connect_fd > server_nfds)
				server_nfds = connect_fd;

			if (i > server_num)
				server_num = i;

			if (--server_nready <= 0)
				continue;
		}

		/* new client connection */
		if (FD_ISSET(connect_client_fd, &client_readfds)) {
			client_addrlen = sizeof(struct sockaddr_storage);
			connect_fd = accept(connect_client_fd, (struct sockaddr *) &client_addr, &client_addrlen);

			for (i = 0; i < FD_SETSIZE; ++i) {
				/* save connect client fd */
				if (client[i].fd != -1)
					continue;

				client[i].fd = connect_fd;
				snprintf(client[i].ip, sizeof(client[i].ip), "%s",
					inet_addrstr((struct sockaddr *) &client_addr, client_addrlen));
				printf("Client(%d) connected, ip = %s\n", i, client[i].ip);
				break;
			}

			/* 连接客户不能大于FD_SETSIZE */
			if (i == FD_SETSIZE)
				fprintf(stderr, "Too many clients\n");

			/* add new client fd to set */
			FD_SET(connect_fd, &client_allfds);

			if (connect_fd > client_nfds)
				client_nfds = connect_fd;

			if (i > client_num)
				client_num = i;

			if (--client_nready <= 0)
				continue;
		}



		/* check all client */
		for (i = 0; i <= client_num; ++i) {
			if (client[i].fd < 0)
				continue;

			memset(client[i].data, 0, sizeof(client[i].data));
			num_bytes = 0;

			// 有客户连接，检测是否有数据
			if (FD_ISSET(client[i].fd, &client_readfds)) {
				num_bytes = read(client[i].fd, client[i].data, sizeof(client[i].data));

				if (num_bytes <= 0) {
					close(client[i].fd);
					//从监听集合中删除此socket连接
					FD_CLR(clinet[i].fd, &client_allfds);
					memset(&client[i], 0, sizeof(&client[i]));
					client[i].fd = -1;
				}

				//process_recv_data();

			}
		}

		sleep(1);
	}

	close(socket_fd);

}




/*
 *
 */
static void *
update_data(void *arg)
{
	char s1[] = "01 | 001 | 24.0 'C | 43.2 %RH | 0 | 0 \r\n";
	char s2[] = "01 | 002 | 26.0 'C | 45.0 %RH | 0 | 0 \r\n";
	char s3[] = "02 | 010 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 \r\n";
	char s4[] = "03 | 005 | 230 | 228 | 228 | 221 | 222 | 222 | 230 | 230 | 230 | 015 | 013 | 018 | 3315 | 2886 | 3996 | 420 | 421 | 000 | 003 | 49.9 \r\n";
	char s5[] = "04 | 006 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 \r\n";

	while (1) {

		/* update device data */


		/* check all servers */
		for (i = 0; i <= server_num; ++i) {
			if (server[i].fd <0)
				continue;

			num_bytes = 0;

			// 有服务器连接，检测是否有数据
			if (FD_ISSET(server[i].fd, &server_readfds)) {
				num_bytes = read(server[i].fd, server[i].data, sizeof(server[i].data));

				if (num_bytes <= 0) {
					close(server[i].fd);
					//从监听集合中删除此socket连接
					FD_CLR(server[i].fd, &server_allfds);
					memset(&server[i], 0, sizeof(&server[i]));
					server[i].fd = -1;
				}

				//process_recv_data();

			}
		}


		/* check all clients */
		for (i = 0; i <= client_num; ++i) {
			if (client[i].fd < 0)
				continue;

			num_bytes = 0;

			/* 有客户连接，发送数据 */
			if (client[i].fd != -1) {
				// send device data();

				//num_bytes = write(fd, buf, strlen(buf));

				//if (num_bytes <= 0) {
				//	close(fd);
					//从监听集合中删除此socket连接
				//	FD_CLR(fd, &allfds);
				//	client[i] = -1;
				//	fd = -1;
				}
			}
		}
	}
	
}








