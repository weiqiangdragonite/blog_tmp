/*
 * 通过完整示例来理解如何使用 epoll
 * http://blog.jobbole.com/93566/
 */

/*
网络服务器通常使用一个独立的进程或线程来实现每个连接。由于高性能应用程序需要同时
处理大量的客户端，这种方法就不太好用了，因为资源占用和上下文切换时间等因素影响了
同时处理大量客户端的能力。另一种方法是在一个线程中使用非阻塞 I/O，以及一些就绪通
知方法，即当你可以在一个套接字上读写更多数据的时候告诉你。


本文介绍了 Linux 的 epoll(7) 机制，它是 Linux 最好的就绪通知机制。我们用 C 语言
编写了示例代码，实现了一个完整的 TCP 服务器。 我假设您有一定 C 语言编程经验，知
道如何在 Linux 上编译和运行程序，并且可以阅读手册查看各种需要的 C 函数。


epoll 是在 Linux 2.6 中引入的，在其他类 UNIX 操作系统上不可用。它提供了一个类似
于 select(2) 和 poll(2) 函数的功能：

1、select(2) 一次可以监测 FD_SETSIZE数量大小的描述符，FD_SETSIZE 通常是一个在
 libc 编译时指定的小数字。
2、poll(2) 一次可以监测的描述符数量并没有限制，但撇开其它因素，我们每次都不得不
检查就绪通知，线性扫描所有通过描述符，这样时间复杂度为 O(n)而且很慢。


epoll 没有这些固定限制，也不执行任何线性扫描。因此它可以更高效地执行和处理大量
事件。

一个 epoll 实例可由 epoll_create(2) 或 epoll_create1(2) （它们采用不同的参数）
创建，它们的返回值是一个 epoll 实例。epoll_ctl(2) 用来添加或删除监听 epoll 实例
的描述符。epoll_wait(2) 用来等待被监听的描述符事件，一直阻塞到事件可用。更多信息
请参见相关手册。

当描述符被添加到 epoll 实例时，有两种模式：电平触发和边缘触发（译者注：借鉴电路
里面的概念）。当你使用电平触发模式，并且数据可以被读取，epoll_wait(2) 函数总是会
返回就绪事件。如果你还没有读完数据，并且再次在 epoll 实例上调用 epoll_wait(2) 
函数监听这个描述符，由于还有数据可读，那么它会再次返回这个事件。在边缘触发模式
下，你只会得到一次就绪通知。如果你没有将数据全部读走，并且再次在 epoll 实例上调
用 epoll_wait(2) 函数监听这个描述符，它就会阻塞，因为就绪事件已经发送过了。


传递到 epoll_ctl(2) 的 epoll 事件结构体如下。对每一个被监听的描述符，你可以关联
到一个整数或者一个用户数据的指针。

typedef union epoll_data
{
  void        *ptr;
  int          fd;
  __uint32_t   u32;
  __uint64_t   u64;
} epoll_data_t;
 
struct epoll_event
{
  __uint32_t   events; /* Epoll events * /
  epoll_data_t data;   /* User data variable * /
};

*/



/*
现在我们开始写代码。我们将实现一个小的 TCP 服务器，将发送到这个套接字的所有数据
打印到标准输出上。首先编写一个 create_and_bind() 函数，用来创建和绑定 TCP 套接字。

create_and_bind() 包含一个标准代码块，用一种可移植的方式来获得 IPv4 和 IPv6 
套接字。它接受一个 port 字符串参数，可由 argv[1] 传递。getaddrinfo(3) 函数返回
一堆 addrinfo 结构体到 result 变量中，它们与传入的 hints参数是兼容的。addrinfo
结构体像这样：
struct addrinfo
{
  int              ai_flags;
  int              ai_family;
  int              ai_socktype;
  int              ai_protocol;
  size_t           ai_addrlen;
  struct sockaddr *ai_addr;
  char            *ai_canonname;
  struct addrinfo *ai_next;
};

我们依次遍历这些结构体并用它们创建套接字，直到可以创建并绑定一个套接字。如果
成功了，create_and_bind() 返回这个套接字描述符。如果失败则返回 -1。
*/
static int
create_and_bind(char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int i, sfd;

	/* 设置监听服务的基本信息 */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;		/* IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM;	/* tcp socket */
	hints.ai_flags = AI_PASSIVE;		/* open passive socket */

	i = getaddrinfo(NULL, port, &hints, &result);
	if (i != 0) {
		fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(i));
		return -1;
	}

	i = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->si_protocol);

		/* error, try next address */
		if (sfd == -1)
			continue;

		
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i))
			== -1)
		{
			perror("setsockopt() failed");
			close(sfd);
			freeaddrinfo(result);
			return -1;
		}

		/* bind success */
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;

		/* close socket and try next address */
		close(sfd);
	}

	if (rp == NULL)
		fprintf(stderr, "Could not bind socket to any address\n");

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : sfd;
}



/*
下面我们编写一个函数，用于将套接字设置为非阻塞状态。make_socket_non_blocking() 
为传入的 sfd 参数设置 O_NONBLOCK 标志：
*/
static int
make_socket_non_blocking(int sfd)
{
	int flags, s;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl()-GETFL failed");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1) {
		perror("fcntl()-SETFL failed");
		return -1;
	}

	return 0;
}



#define MAX_EVENTS	64


int
main(int argc, char *argv[])
{
	int sfd, s;
	int epollfd;
	struct epoll_event event;
	struct epoll_event *wait_events;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(-1);
	}

	sfd = create_and_bind(argv[1]);
	if (sfd == -1)
		exit(-1);

	s = make_socket_non_blocking(sfd);
	if (s == -1)
		exit(-1);

	s = listen(sfd, SOMAXCONN);
	if (s == -1) {
		perror("listen() failed");
		exit(-1);
	}


	epollfd = epoll_creat
}


