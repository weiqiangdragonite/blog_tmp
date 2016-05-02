///YS SOCKET客户端，连接到服务器那边以获取数据包
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "packet_info.h"
extern char server_ip[32];

extern int rawpacket_putin_buff(const int len ,const u_char * packet);


/*******************
客户端，使用6789端口通信，要审计的包则是通过该端口进行传输
********************/
int sock_client()
{
     int sockfd;
    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct sock_buff *my_sock_buff;
    struct arp_user *arp_user_temp;
    char buff[sizeof(struct sock_buff)];

    int portno = 6789;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)         printf("ERROR opening socket  server_ip  :%s\n",server_ip);
    server = gethostbyname(server_ip);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,          (char *)&serv_addr.sin_addr.s_addr,         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)         printf("ERROR connecting \n");
    while(1)
    {
       // n = read(sockfd,(void*)buff,sizeof(struct sock_buff));
       //dbug("OOOOOOOOOOOOOOOOOOo \n");
        n = recv(sockfd,(void*)buff,1544,MSG_WAITALL);///YS ，不要用READ，不然会出现不完整的包，不过这函数是阻塞函数，在没有读到1544时，会阻塞在那里
        if (n <= 0)
        {
            printf("ERROR reading from socket  %s\n",server_ip);
            exit(0);
        }
        else
        {
            if(buff[0]=='v' && buff[1]=='v' &&  buff[2] =='v' && buff[3]=='v')
            {
                arp_user_temp=(struct arp_user*) buff;
                creat_arp_user_list(arp_user_temp);
            }

            if(!strncmp("reboot",buff,6))
            {
				system("pkill MYP");
				system("/mg/MYP/MYP &");
                dbug("reboot\n");
                exit(0);
            }
            my_sock_buff=(struct sock_buff*) buff;
         //   dbug("OOOOOOOOOOOOOOOOOOo %d\n",my_sock_buff->len);
           rawpacket_putin_buff (my_sock_buff->len,my_sock_buff->raw_packet);///YS 写入缓冲区
        }
        memset(buff,0,sizeof(struct sock_buff));
    }
    close(sockfd);
    return 0;
}
