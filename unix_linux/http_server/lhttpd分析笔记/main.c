/*
GazTek HTTP Daemon (ghttpd)
Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <sys/time.h> 
#include <unistd.h> 
#include <signal.h>
#include <pwd.h>
#include <dirent.h>
#include <syslog.h>
              
#include "ghttpd.h"

void signal_handler(int signum);
int inetd_server();

unsigned long no_vhosts = 0;
t_vhost *vhosts = 0;

unsigned int SERVERPORT = 80;
char SERVERTYPE[255] = "Standalone";
char SERVERROOT[255] = "/usr/local/ghttpd";
char PARSETYPES[255] = ".shtml .splice";
char ALLOWSQL[255] = "yes";
FILE * fpid;
t_vhost defaulthost;                              //服务器的所有配置信息，资源信息等！
char * SERVERIP;

int main()
{
	int sockfd, new_fd;  /* listen on sock_fd, new connection on new_fd */
	int xx=0, numbytes=0;
	
	struct sockaddr_in my_addr;    /* my address information */
	struct sockaddr_in their_addr; /* connector's address information */
	
	int sin_size, i;
	struct timeval tv;
	fd_set readfds, exceptfds;
	struct passwd *pw;

	/*
	 * Setup the default values
	 */

	strcpy(defaulthost.DOCUMENTROOT, "/usr/local/lhttpd/htdocs");
	strcpy(defaulthost.DEFAULTPAGE, "index.html");
	strcpy(defaulthost.CGIBINDIR, "/cgi-bin");
	strcpy(defaulthost.CGIBINROOT, "/usr/local/lhttpd/cgi-bin");

	/*
	 * Count the virtual hosts and allocate the memory
	 */

	no_vhosts = count_vhosts();                                     //计算虚拟主机的个数
	vhosts = (t_vhost *)malloc((no_vhosts+1) * sizeof(t_vhost));    //为每个虚拟主机分配存储信息的结构对象，存储在t_vhost *动态数组中

	/*
	 * Read in all the virtual hosts and other configuration
	 */

	readinconfig();                                                 //知道virtual host并分配的存储空间后，读入信息

	if(!strcmp(SERVERTYPE, "inetd"))
		return inetd_server();                                       // 服务类型 默认是standalone

	/*
	 * Setup the sockets and wait and process connections
	 */

	openlog("lhttpd", (char)NULL, LOG_DAEMON);                         //打开一个到系统日志记录程序的链接

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {           //创建一个socket，STREAM-有序，可靠，面向链接，双向
		perror("socket");
		syslog(LOG_CRIT, "Could create a socket, exiting\n");
		closelog();
		exit(1);
	}

	/* Let the kernel reuse the socket address. This lets us run
           twice in a row, without waiting for the (ip, port) tuple
           to time out. */
        i = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&i, i);    //设置socket选项让内核重用socket地址
	                                      

	my_addr.sin_family = AF_INET;         /* host byte order */
	my_addr.sin_port = htons(SERVERPORT);     /* short, network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */              //自动设置ip
	bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */        //把结构内剩余的字段填空
	
	SERVERIP = (char *)my_addr.sin_addr.s_addr;                                   //SERVER IP
	
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)   //将socket绑定到这个addr上
	{
		perror("bind");
		syslog(LOG_CRIT, "Could not bind to port, exiting\n");
		closelog();
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {                                              //开始监听
		perror("listen");
		syslog(LOG_CRIT, "Unable to do a listen()\n");
		closelog();
		exit(1);
	}

	// Set up signal handlers so we can clear up our child processes
	signal(SIGTERM, signal_handler);                                                   //设置信号，信号处理程序
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGCHLD, signal_handler);

	xx = fork();
	if(xx==-1)
	{
		printf("FATAL ERROR, Could not fork() lhttpd into background\n");
		syslog(LOG_CRIT, "Could not fork() lhttpd into background\n");
		closelog();
		exit(1);
	}
	if(xx != 0)	// Parent...                                                    //父进程退出，将服务进程移交给子进程来处理，也就将该进程作为后台进程来运行
		exit(0);
		
	printf("\nLHTTPd Running...\n");
	/* Write the pid file so the script can start and stop it */
	fpid = fopen("/usr/local/lhttpd/pid", "w");                                              //为了让脚本可以在运行时根据进程id号来关闭进程，所以要写入进程号
	fprintf(fpid, "%d", getpid());
	printf("Use /usr/local/lhttpd/light stop to shutdown the server.\nPress any key.\n");
	fclose(fpid);

	closelog();	// End of all the syslog errors, anything else written to ghttpd.log

	/* drop any priveledges we have */                                                          //进程权限设置
	if ((pw = getpwnam("nobody")) != NULL) {
		setgid(pw->pw_gid);
		setuid(pw->pw_uid);
	}

	while(1) {  /* main accept() loop */
		sin_size = sizeof(struct sockaddr_in);

		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)         //接收握手链接，获得socket的文件描述符
			continue;

		if (!fork()) { /* this is the child process */                                         //fork如果返回0，表示下面为新的子进程的处理过程
			while(1)
			{
				close(sockfd);                                                                 //子处理进程不需要父进程的监听socket
				FD_ZERO(&readfds);															   //将指定的文件描述符集合清空
				FD_SET(new_fd, &readfds); 													   //将FD加入到set中
				FD_ZERO(&exceptfds);
				FD_SET(new_fd, &exceptfds);
				
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				
				select(new_fd+1, &readfds, NULL, &exceptfds, &tv);                             //select使得进程可以同时等待多个IO设备
				//参数一指定了文件描述符集中要被检测的比特数，这个值必须至少比待检测的最大文件描述符大1
				//参数二指定了被读监控的文件描述符集
				//参数三指定了被写监控的文件描述符集
				//参数四指定了被例外条件监控的文件描述符集
				//参数五起了定时器作用，到了指定时间，无论设备是否准备好，都返回调用。
				
				if(FD_ISSET(new_fd, &exceptfds))      											//检测new_fd位是否为真
					break;
				
				if(FD_ISSET(new_fd, &readfds))                                                  //一旦前面的select中有了返回，那么这里就检测是不是当前的子进程的fd位被设置了，如果是则开始连接返回数据
				{
					setenv("REMOTE_ADDR", (char *)inet_ntoa(their_addr.sin_addr), 1);           //将当前进程的系统变量REMOTE_ADDR设置为客户端的ip地址，并用1表示更新
					if(serveconnection(new_fd)==-1) break;                                      //连接服务器
				}
				else
					break;
					
			}
			
			close(new_fd);
			exit(0);
		}
		close(new_fd);  /* parent doesn't need this */                                         //父进程不需要干预子进程，所以fork返回非0时直接关闭
		
		while(waitpid(-1,NULL,WNOHANG) > 0); /* clean up child processes */                    //子进程结束时清除，防止成为僵尸进程
	}

	return 0;
}


/*
 * This func is called if the server type is 'inetd' after the config has been read
 */
int inetd_server()
{
  struct sockaddr_in their_addr; /* connector's address information */
  size_t sval = sizeof(their_addr); 
  struct passwd *pw;

  /* not really needed if using tcp_wrappers */
  if (getpeername(0, (struct sockaddr *) &their_addr, &sval) < 0) {
	 exit(1);
  }

  /* drop any priveledges we have */
  if ((pw = getpwnam("nobody")) != NULL) {
	 setgid(pw->pw_gid);
	 setuid(pw->pw_uid);
  }

  Log("Connection from %s", inet_ntoa(their_addr.sin_addr));
  setenv("REMOTE_ADDR", (char *)inet_ntoa(their_addr.sin_addr), 1);

  serveconnection(0);

  return 0;
}

void signal_handler(int signum)
{
	if(signum == SIGCHLD)
	{
		while(waitpid(-1,NULL,WNOHANG) > 0); /* clean up child processes */
	}
	else if(signum == SIGHUP)
		readinconfig();
	else
	{
		while(waitpid(-1,NULL,WNOHANG) > 0);
		exit(0);
	}
}

