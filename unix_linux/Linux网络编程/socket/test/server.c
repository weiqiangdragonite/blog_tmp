/*
 * !!! BAD STYLE !!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>



int sockfd, newsockfd;

int
main(int argc, char *argv[])
{
	sock_server();
	return 0;
}

int sock_server()
{
    signal(SIGPIPE,SIG_IGN);///YS 就算客户端掉线了，我们的服务端也不退出
    int portno=10000;///YS 我们固定的端口是在6789，这个暂时还没在数据库上配置（放后做）
    int flag=1,flag_len=sizeof(int);
    char *data = "hello, world\r\n";

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "socket() error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&flag,flag_len)<0)
    {
        fprintf(stderr, "ERROR ON setockopt\n");
	exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "ERROR on binding\n");
	exit(EXIT_FAILURE);
    }

    listen(sockfd,5);
    while(1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,     (struct sockaddr *) &cli_addr,  &clilen);
        if (newsockfd < 0)
        {
		fprintf(stderr, "ERROR on accept\n");
		exit(EXIT_FAILURE);
        }


          while(1)
          {
              if (write(newsockfd,data, strlen(data)) < 0)
              {
		if (errno == EPIPE)
                  fprintf(stderr, "Socket had closed\n");
		else
			fprintf(stderr, "write() failed: %s\n", strerror(errno));

                  close(newsockfd);
		newsockfd=-1;
                  break;
              }
		sleep(1);
          }
    }
    close(sockfd);
    return 0;
}



