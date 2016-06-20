/*
 * 通过完整示例来理解如何使用 epoll
 * http://blog.jobbole.com/93566/
 *
 * 参考
 * http://www.cnblogs.com/aicro/archive/2012/12/27/2836170.html
 * http://blog.chinaunix.net/uid-20583479-id-1920065.html
 * http://blog.csdn.net/feitianxuxue/article/category/1111233  处理大并发
 * http://blog.csdn.net/huangjm_13/article/details/17676591 epoll epoll事件类型
 *
 * 例子编译ok，但没测试
 */




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>





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

struct epoll_event
{
  __uint32_t   events; / * Epoll events * /
  epoll_data_t data;   / * User data variable * /
};


typedef union epoll_data
{
  void        *ptr;
  int          fd;
  __uint32_t   u32;
  __uint64_t   u64;
} epoll_data_t;
 

*/



/*
现在我们开始写代码。我们将实现一个小的 TCP 服务器，将发送到这个套接字的所有数据
打印到标准输出上。首先编写一个 create_and_bind() 函数，用来创建和绑定 TCP 
套接字。

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
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

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


static int
process_new_connection(int sfd)
{
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	struct sockaddr_in cli_addr;
	socklen_t socklen;
	int clifd, ret;

	
	socklen = sizeof(cli_addr);
	clifd = accept(sfd, (struct sockaddr *) &cli_addr, &socklen);

	if (clifd == -1) {
		if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			/* client connect not ready */
			/* 这里应该不会出现的吧? */
			return -1;
		} else {
			perror("accept() failed");
			return -1;
		}
	}

	ret = getnameinfo((struct sockaddr *) &cli_addr, socklen,
		host, NI_MAXHOST, service, NI_MAXSERV, 0);
	if (ret == 0)
		printf("Accepted connection from: %s (%s)\n", host, service);


	/* make the client fd to non-blocking */
	if (make_socket_non_blocking(clifd) == -1)
		return -1;


	return clifd;
}


static int
add_epoll_event_list(struct epoll_event *event, int fd, int epollfd)
{
	/* 设置与要处理的事件相关的文件描述符 */
	event->data.fd = fd;
	/* 设置要处理的事件类型为 边缘触发 */
	/* 边缘出发时需要设置为非阻塞的 */
	event->events = EPOLLIN | EPOLLET;

	/* 注册epoll事件 */
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, event) == -1) {
		perror("epoll_ctl() - EPOLL_CTL_ADD failed");
		return -1;
	}

	return 0;
}


static void
process_client_data(int clifd)
{
	char buf[1024];
	ssize_t nbytes;
	int done = 0;

	/* read all the data */
	while (1) {
		memset(buf, 0, sizeof(buf));
		nbytes = read(clifd, buf, sizeof(buf) - 1);

		if (nbytes == -1) {
			if (errno == EAGAIN) {
				/* 由于是非阻塞的模式,所以当errno为EAGAIN时,
				 * 表示当前缓冲区已无数据可读,在这里就当作是
				 * 该次事件已处理处. */
				done = 1;
				break;
			} else if (errno == ECONNRESET) {
				/* 对方发送了RST */
				break;
			} else if (errno == EINTR) {
				/* 信号中断 */
				continue;
			} else {
				perror("read() failed");
				break;
			}
		} else if (nbytes == 0) {
			/* end of file or client closed connection*/
			/* close socket */
			printf("client closed\n");
			break;
		}

		/* write to standard output */
		if (write(STDIN_FILENO, buf, nbytes) != nbytes)
			perror("write() failed");
	}
}





#define MAX_EVENTS	64


int
main(int argc, char *argv[])
{
	int sfd, clifd;
	int epollfd, event_cnt;
	int timeout, i;

	struct epoll_event event;
	struct epoll_event *wait_events;


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(-1);
	}

	sfd = create_and_bind(argv[1]);
	if (sfd == -1)
		exit(-1);


	if (make_socket_non_blocking(sfd) == -1)
		exit(-1);


	if (listen(sfd, SOMAXCONN) == -1) {
		perror("listen() failed");
		exit(-1);
	}


	/* epoll_create1(0) 代替epoll_create(size) */
	epollfd = epoll_create1(0);
	if (epollfd == -1) {
		perror("epoll_create1() failed");
		exit(-1);
	}

	/* buffer where events are returned */
	wait_events = calloc(MAX_EVENTS, sizeof(struct epoll_event));
	if (wait_events == NULL) {
		perror("calloc() failed");
		exit(-1);
	}

	if (add_epoll_event_list(&event, sfd, epollfd) == -1)
		exit(-1);



	/* event loop 开始循环 */
	while (1) {
		/* -1是阻塞，0是不会阻塞，大于0至多阻塞timeout毫秒 */
		timeout = -1;

		event_cnt = epoll_wait(epollfd, wait_events, MAX_EVENTS,
					timeout);
		if (event_cnt == -1) {
			fprintf(stderr, "epoll_wait() failed\n");
			break;
		} else if (event_cnt == 0) {
			printf("time out!\n");
			continue;
		}

		/* 处理发生事件 */
		for (i = 0; i < event_cnt; ++i) {
			/* an error has occured on this fd */
			/* 关于错误请看下面的注释 */
			if ((wait_events[i].events & EPOLLERR)
				|| (wait_events[i].events & EPOLLHUP))
			{
				fprintf(stderr, "error\n");
				epoll_ctl(epollfd, EPOLL_CTL_DEL,
					wait_events[i].data.fd, NULL);
				close(wait_events[i].data.fd);
				continue;
			}

			/* new incoming connections */
			else if (wait_events[i].data.fd == sfd) {
				clifd = process_new_connection(sfd);

				if (clifd != -1)
					add_epoll_event_list(&event, clifd,
								epollfd);
			}

			/* We have data on the fd waiting to be read. Read and
			 * display it. We must read whatever data is available
			 * completely, as we are running in edge-triggered mode
			 * and won't get a notification again for the same
			 * data. */
			/* 如果是已经连接的用户，并且收到数据，那么进行读入 */
			else if (wait_events[i].events & EPOLLIN) {
				clifd = wait_events[i].data.fd;
				if (clifd > 0)
					process_client_data(clifd);
			}

			/* 如果有数据发送，这个不需要吧，真有数据要发送直接发就行了 */
			else if(wait_events[i].events & EPOLLOUT) {
				continue;
			}
		}
	}

	free(wait_events);
	close(sfd);

	return 0;
}

/*
main() 首先调用 create_and_bind() 新建套接字。然后把套接字设置非阻塞模式，
再调用listen(2)。接下来它创建一个 epoll 实例 efd，添加监听套接字 sfd ，用
电平触发模式来监听输入事件。

外层的 while 循环是主要事件循环。它调用epoll_wait(2)，线程保持阻塞以等待事件
到来。当事件就绪，epoll_wait(2) 用 events 参数返回事件，这个参数是一群
 epoll_event 结构体。

当我们添加新的监听输入连接以及删除终止的现有连接时，efd 这个 epoll 实例在事件
循环中不断更新。

当事件是可用的，它们可以有三种类型：

错误：当一个错误连接出现，或事件不是一个可以读取数据的通知，我们只要简单地
      关闭相关的描述符。关闭描述符会自动地移除 efd 这个 epoll 实例的监听列表。
新连接：当监听描述符 sfd 是可读状态，这表明一个或多个连接已经到达。当有一个
        新连接， accept(2) 接受这个连接，打印一条相应的消息，把这个到来的套接字
        设置为非阻塞状态，并将其添加到 efd 这个 epoll 实例的监听列表。
客户端数据：当任何一个客户端描述符的数据可读时，我们在内部 while 循环中用
            read(2) 以 512 字节大小读取数据。这是因为当前我们必须读走所有可读的
            数据，当监听描述符是边缘触发模式下，我们不会再得到事件。被读取的
            数据使用 write(2) 被写入标准输出(fd=1)。如果 read(2) 返回 0，
            这表示 EOF 并且我们可以关闭这个客户端的连接。如果返回 -1，errno 
            被设置为 EAGAIN，这表示这个事件的所有数据被读走，我们可以返回主循环。

就是这样。它在一个循环中运行，在监听列表中添加和删除描述符。





epoll 事件的检测


1.

epoll 事件中发现
就EPOLLIN , EPOLLOUT , EPOLLPRI可以用.

EPOLLERR 和 EPOLLHUP什么情况下才能监测出这种问题啊.

我的内核是2.6.20
可是用EPOLLRDHUP的时候编译包错.
RecvMessThread.cpp:48: error: ‘EPOLLRDHUP’ was not declared in this scope


2.

1、listen fd，有新连接请求，触发EPOLLIN。
2、对端发送普通数据，触发EPOLLIN。
3、带外数据，只触发EPOLLPRI。
4、对端正常关闭（程序里close()，shell下kill或ctr+c），触发EPOLLIN和EPOLLRDHUP，但是不触发EPOLLERR和EPOLLHUP。
    关于这点，以前一直以为会触发EPOLLERR或者EPOLLHUP。
    再man epoll_ctl看下后两个事件的说明，这两个应该是本端（server端）出错才触发的。
5、对端异常断开连接（只测了拔网线），没触发任何事件。


附man：

EPOLLIN       连接到达；有数据来临；
              The associated file is available for read(2) operations.
EPOLLOUT      有数据要写
              The associated file is available for write(2) operations.
EPOLLRDHUP    这个好像有些系统检测不到，可以使用EPOLLIN，read返回0，删除掉事件，关闭close(fd);
              如果有EPOLLRDHUP，检测它就可以直到是对方关闭；否则就用上面方法。
              Stream socket peer closed connection, or shut down writing half
              of connection. (This flag is especially useful for writing sim-
              ple code to detect peer shutdown when using Edge Triggered moni-
              toring.)
EPOLLPRI      外带数据
              There is urgent data available for read(2) operations.

              
EPOLLERR      只有采取动作时，才能知道是否对方异常。即对方突然断掉，是不可能
              有此事件发生的。只有自己采取动作（当然自己此刻也不知道），read，
              write时，出EPOLLERR错，说明对方已经异常断开。
              
              EPOLLERR 是服务器这边出错（自己出错当然能检测到，对方出错你咋能
              直到啊）
              
              Error condition happened on the associated file descriptor.
              epoll_wait(2) will always wait for this event; it is not neces-
              sary to set it in events.
              
EPOLLHUP
              Hang up   happened   on   the   associated   file   descriptor.
              epoll_wait(2) will always wait for this event; it is not neces-
              sary to set it in events.
              
EPOLLET       边缘触发模式
              Sets the Edge Triggered behavior for the associated file
              descriptor.   The default behavior for epoll is Level Triggered.
              See epoll(7) for more detailed information about Edge and Level
              Triggered event distribution architectures.
              
EPOLLONESHOT (since Linux 2.6.2)
              Sets the one-shot behavior for the associated file descriptor.
              This means that after an event is pulled out with epoll_wait(2)
              the associated file descriptor is internally disabled and no
              other events will be reported by the epoll interface. The user
              must call epoll_ctl() with EPOLL_CTL_MOD to re-enable the file
              descriptor with a new event mask.

关于EPOLLERR：
！！！！！！socket能检测到对方出错吗？目前为止，好像我还不知道如何检测。
但是，在给已经关闭的socket写时，会发生EPOLLERR，也就是说，只有在采取行动（比如
读一个已经关闭的socket，或者写一个已经关闭的socket）时候，才知道对方是否关闭了。
这个时候，如果对方异常关闭了，则会出现EPOLLERR，出现Error把对方DEL掉，close就可以
了。！！！！！！！

关于EPOLLHUP：
！！！！！！socket能检测到对方出错吗？目前为止，好像我还不知道如何检测。
但是，在给已经关闭的socket写时，会发生EPOLLERR，也就是说，只有在采取行动（比如
读一个已经关闭的socket，或者写一个已经关闭的socket）时候，才知道对方是否关闭了。
这个时候，如果对方异常关闭了，则会出现EPOLLERR，出现Error把对方DEL掉，close就可以
了。！！！！！！！

3.各类事件

1）监听的fd，此fd的设置等待事件：
    EPOLLIN ；或者EPOLLET |EPOLLIN 
    
    由于此socket只监听有无连接，谈不上写和其他操作。
    故只有这两类。（默认是LT模式，即EPOLLLT |EPOLLIN）。
    
    说明：如果在这个socket上也设置EPOLLOUT等，也不会出错，
    只是这个socket不会收到这样的消息。

2）客户端正常关闭
client 端close()联接

server 会报某个sockfd可读，即epollin来临。 
然后recv一下 ， 如果返回0再掉用epoll_ctl 中的EPOLL_CTL_DEL , 同时close(sockfd)。

有些系统会收到一个EPOLLRDHUP，当然检测这个是最好不过了。只可惜是有些系统，
上面的方法最保险；如果能加上对EPOLLRDHUP的处理那就是万能的了。


3）客户端异常关闭：

     客户端异常关闭，并不会通知服务器（如果会通知，以前的socket当然会有与此相关
     的api）。正常关闭时read到0后，异常断开时检测不到的。服务器再给一个已经关闭
     的socket写数据时，会出错，这时候，服务器才明白对方可能已经异常断开了（读也
     可以）。
     
     Epoll中就是向已经断开的socket写或者读，会发生EPollErr，即表明已经断开。

4）EpollIn：
     
     
     
5）监听的skocket只需要EpollIn就足够了，EpollErr和EpollHup会自动加上。
    监听的socket又不会写，一个EpollIn足矣。
    
        
4. 补充 EpollErr

当客户端的机器在发送“请求”前，就崩溃了（或者网络断掉了），则服务器一端是无从知晓的。

按照你现在的这个“请求响应方式”，无论是否使用epoll，都必须要做超时检查。

因此，这个问题与epoll无关。

因此，EpollErr这种错误必须是有动作才能检测出来。
服务器不可能经常的向客户端写一个东西，依照有没有EpollErr来判断
客户端是不是死了。

因此，服务器中的超时检查是很重要的。这也是以前服务器中作死后确认的原因。
新的代码里也是时间循环，时间循环....

！！！服务器中的超时检查！！！很重要

*/






