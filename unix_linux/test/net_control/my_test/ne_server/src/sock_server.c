///YS SOCKET服务器端，这个服务器端是用来发送数据到审计那边的

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
extern void dbug(char *fmt,...);
int sockfd, newsockfd;

int sock_server( )
{
    signal(SIGPIPE,SIG_IGN);///YS 就算客户端掉线了，我们的服务端也不退出
    int portno=6789;///YS 我们固定的端口是在6789，这个暂时还没在数据库上配置（放后做）
    int flag=1,flag_len=sizeof(int);
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        dbug("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&flag,flag_len)<0)
    {
        dbug("ERROR ON setockopt");
    }
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        dbug("ERROR on binding");
    }

    listen(sockfd,5);
    while(1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,     (struct sockaddr *) &cli_addr,  &clilen);
        if (newsockfd < 0)
        {
            dbug("ERROR on accept");
        }
        /*  while(1)
          {
              n= write(newsockfd,"1234567890-",11);
              if (n < 0)
              {
                  dbug("ERROR writing to socket");
                  close(newsockfd);
                  break;
              }
          }*/
    }
    close(sockfd);
    return 0;
}


