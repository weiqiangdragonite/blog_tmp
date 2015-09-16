
// 下面是客户的心搏函数
#include "unp.h"
/* 给heartbeat_cli的参数的拷贝: 套接口描述字(信号处理程序需用它来发送和接收带外数据)，SIGALRM的频率，在客户认为服务器或连接死掉之前没有来子服务器的响应的SIGALRM的总数，总量nprobes记录从最近一次服务器应答以来的SIGALRM的数目 */
static int servfd; 
static int nsec;  /* #seconds between each alarm */
static int maxnprobes;  /* #probes w/no response before quit */
static int nprobes;  /* #probes since last server response */
static void sig_urg(int), sig_alrm(int);
void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg)
{/* heartbeat_cli函数检查并且保存参数，给SIGURG和SIGALRM建立信号处理函数，将套接口的属主设为进程ID，alarm调度一个SIGALRM */
  servfd = servfd_arg;  /* set globals for signal handlers */
  if( (nsec = nsec_arg) < 1)
    nsec = 1;
  if( (maxnprobes = maxnprobes_arg) < nsec)
    maxnprobes = nsec;
  nprobes = 0;
  Signal(SIGURG, sig_urg);
  Fcntl(servfd, F_SETOWN, getpid() );
  Signal(SIGALRM, sig_alrm);
  alarm(nesc);
}
static void sig_urg(int signo)
{/* 当一个带外通知到来时，就会产生这个信号。我们试图去读带外字节，但如果还没有到(EWOULDBLOCK)也没有关系。由于系统不是在线接收带外数据，因此不会干扰客户读取它的普通数据。既然服务器仍然存活，nprobes就重置为0 */
  int n;
  char c;
  if( ( n = recv(servfd, &c, 1, MSG_OOB) ) < 0 )
  {
    if(errno != EWOULDBLOCK)
      err_sys("recv error");
  }
  nprobes = 0;  /* reset counter */
  return;  /* may interrupt client code */
}
static void sig_alrm(int signo)
{/* 这个信号以有规律间隔产生。计数器nprobes增1， 如果达到了maxnprobes，我们认为服务器或者崩溃或者不可达。在这个例子中，我们结束客户进程，尽管其他的设计也可以使用：可以发送给主循环一个信号，或者作为另外一个参数给heartbeat_cli提供一个客户函数，当服务器看来死掉时调用它 */
  if( ++nprobes > maxnprobes)
  {
    fprintf(stderr, "server is unreachable \n");
    exit(0);
  }
  Send(servfd, "1", 1, MSG_OOB);
  alarm(nsec);
  return; /* may interrupt client code */
}


/******************************************************************************/
// 下面是服务器程序的心搏函数。

#include "unp.h"
static int servfd;
static int nsec;  /* #seconds between each alarm */
static int maxnalarms; /* #alarms w/no client probe before quit */
static int nprobes;  /* #alarms since last client probe */
static void sig_urg(int), sig_alrm(int);
void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg)
{
  servfd = servfd_arg;  /* set globals for signal handlers */
  if( (nsec = nsec_arg) < 1 )
    nsec = 1;
  if( (maxnalarms = maxnalarms_arg) < nsec)
    maxnalarms = nsec;
  Signal(SIGURG, sig_urg);
  Fcntl(servfd, F_SETOWN, getpid());
  Signal(SIGALRM, sig_alrm);
  alarm(nsec);
}
static void sig_urg(int signo)
{ /* 当一个带外通知收到时， 服务器试图读入它。就像客户一样，如果带外字节没有到达没有什么关系。带外字节被作为带外数据返回给客户。注意，如果recv返回EWOULDBLOCK错误，那么自动变量c碰巧是什么就送给客户什么。由于我们不用带外字节的值，所以这没有关系。重要的是发送1字节的带外数据，而不管该字节是什么。由于刚收到通知，客户仍存活，所以重置nprobes为0 */
  int n;
  char c;
  if( (n = recv(servfd, &c, 1, MSG_OOB)) < 0)
  {
    if(errno != EWOULDBLOCK)
      err_sys("recv error");
  }
  Send(servfd, &c, 1, MSG_OOB);  /* echo back out-of-hand byte */
  nprobes = 0;  /* reset counter */
  return;  /* may interrupt server code */
}
static void sig_alrm(int signo)
{ /* nprobes增1， 如果它到达了调用者指定的值maxnalarms，服务器进程将被终止。否则调度一下SIGALRM */
  if( ++nprobes > maxnalarms)
  {
    printf("no probes from client\n");
    exit(0);
  }
  alarm(nsec);
  return;   /* may interrupt server code */
}


