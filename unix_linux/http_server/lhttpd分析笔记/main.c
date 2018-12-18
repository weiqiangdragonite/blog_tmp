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
t_vhost defaulthost;                              //������������������Ϣ����Դ��Ϣ�ȣ�
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

	no_vhosts = count_vhosts();                                     //�������������ĸ���
	vhosts = (t_vhost *)malloc((no_vhosts+1) * sizeof(t_vhost));    //Ϊÿ��������������洢��Ϣ�Ľṹ���󣬴洢��t_vhost *��̬������

	/*
	 * Read in all the virtual hosts and other configuration
	 */

	readinconfig();                                                 //֪��virtual host������Ĵ洢�ռ�󣬶�����Ϣ

	if(!strcmp(SERVERTYPE, "inetd"))
		return inetd_server();                                       // �������� Ĭ����standalone

	/*
	 * Setup the sockets and wait and process connections
	 */

	openlog("lhttpd", (char)NULL, LOG_DAEMON);                         //��һ����ϵͳ��־��¼���������

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {           //����һ��socket��STREAM-���򣬿ɿ����������ӣ�˫��
		perror("socket");
		syslog(LOG_CRIT, "Could create a socket, exiting\n");
		closelog();
		exit(1);
	}

	/* Let the kernel reuse the socket address. This lets us run
           twice in a row, without waiting for the (ip, port) tuple
           to time out. */
        i = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&i, i);    //����socketѡ�����ں�����socket��ַ
	                                      

	my_addr.sin_family = AF_INET;         /* host byte order */
	my_addr.sin_port = htons(SERVERPORT);     /* short, network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */              //�Զ�����ip
	bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */        //�ѽṹ��ʣ����ֶ����
	
	SERVERIP = (char *)my_addr.sin_addr.s_addr;                                   //SERVER IP
	
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)   //��socket�󶨵����addr��
	{
		perror("bind");
		syslog(LOG_CRIT, "Could not bind to port, exiting\n");
		closelog();
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {                                              //��ʼ����
		perror("listen");
		syslog(LOG_CRIT, "Unable to do a listen()\n");
		closelog();
		exit(1);
	}

	// Set up signal handlers so we can clear up our child processes
	signal(SIGTERM, signal_handler);                                                   //�����źţ��źŴ������
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
	if(xx != 0)	// Parent...                                                    //�������˳�������������ƽ����ӽ���������Ҳ�ͽ��ý�����Ϊ��̨����������
		exit(0);
		
	printf("\nLHTTPd Running...\n");
	/* Write the pid file so the script can start and stop it */
	fpid = fopen("/usr/local/lhttpd/pid", "w");                                              //Ϊ���ýű�����������ʱ���ݽ���id�����رս��̣�����Ҫд����̺�
	fprintf(fpid, "%d", getpid());
	printf("Use /usr/local/lhttpd/light stop to shutdown the server.\nPress any key.\n");
	fclose(fpid);

	closelog();	// End of all the syslog errors, anything else written to ghttpd.log

	/* drop any priveledges we have */                                                          //����Ȩ������
	if ((pw = getpwnam("nobody")) != NULL) {
		setgid(pw->pw_gid);
		setuid(pw->pw_uid);
	}

	while(1) {  /* main accept() loop */
		sin_size = sizeof(struct sockaddr_in);

		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)         //�����������ӣ����socket���ļ�������
			continue;

		if (!fork()) { /* this is the child process */                                         //fork�������0����ʾ����Ϊ�µ��ӽ��̵Ĵ������
			while(1)
			{
				close(sockfd);                                                                 //�Ӵ�����̲���Ҫ�����̵ļ���socket
				FD_ZERO(&readfds);															   //��ָ�����ļ��������������
				FD_SET(new_fd, &readfds); 													   //��FD���뵽set��
				FD_ZERO(&exceptfds);
				FD_SET(new_fd, &exceptfds);
				
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				
				select(new_fd+1, &readfds, NULL, &exceptfds, &tv);                             //selectʹ�ý��̿���ͬʱ�ȴ����IO�豸
				//����һָ�����ļ�����������Ҫ�����ı����������ֵ�������ٱȴ���������ļ���������1
				//������ָ���˱�����ص��ļ���������
				//������ָ���˱�д��ص��ļ���������
				//������ָ���˱�����������ص��ļ���������
				//���������˶�ʱ�����ã�����ָ��ʱ�䣬�����豸�Ƿ�׼���ã������ص��á�
				
				if(FD_ISSET(new_fd, &exceptfds))      											//���new_fdλ�Ƿ�Ϊ��
					break;
				
				if(FD_ISSET(new_fd, &readfds))                                                  //һ��ǰ���select�����˷��أ���ô����ͼ���ǲ��ǵ�ǰ���ӽ��̵�fdλ�������ˣ��������ʼ���ӷ�������
				{
					setenv("REMOTE_ADDR", (char *)inet_ntoa(their_addr.sin_addr), 1);           //����ǰ���̵�ϵͳ����REMOTE_ADDR����Ϊ�ͻ��˵�ip��ַ������1��ʾ����
					if(serveconnection(new_fd)==-1) break;                                      //���ӷ�����
				}
				else
					break;
					
			}
			
			close(new_fd);
			exit(0);
		}
		close(new_fd);  /* parent doesn't need this */                                         //�����̲���Ҫ��Ԥ�ӽ��̣�����fork���ط�0ʱֱ�ӹر�
		
		while(waitpid(-1,NULL,WNOHANG) > 0); /* clean up child processes */                    //�ӽ��̽���ʱ�������ֹ��Ϊ��ʬ����
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

