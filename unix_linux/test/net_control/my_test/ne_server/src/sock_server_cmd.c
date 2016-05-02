///YS 这个服务器端是为了接收PHP那边的操作信息的，现在的话，只操作REBOOT（时间问题，以后慢慢加强，把PHP的操作都记录下来）
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "packet_info.h"
#include <arpa/inet.h>

extern GHashTable *http_hash_white_list;
extern GHashTable *http_hash_person_list;
extern GHashTable *http_hash_group_list;
extern GHashTable *http_hash_whole_list;
extern GHashTable *http_hash_ALL_w_list;
extern GHashTable *http_hash_ALL_b_list;

extern GHashTable *user_hash_list;
extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern GHashTable *tcp_app_tactics_hash_list;
int sockfd_cmd, newsockfd_cmd;
extern void dbug(char *fmt,...);
extern char db_ip[24];
extern int newsockfd;///YS reboot CMD flag
/********************暂时没有使用*******************************************
int create_key()///选择有用的参数，组成HASHKEY
{
    return 0;
}


int message_type(char *buff,    struct cmd_info * cmd )///如果是1－9的类型，返回不同的哈希指针，让MES——DEL和MES——ADD操作
{
    if(strstr(buff,"url"))
    {
        switch(buff[4]-'0')
        {
        case 3:
        {
            dbug("message : white list");
            cmd ->cmd_type=3;
            cmd->p_hash_table=http_hash_white_list;
                break;
        }
        case 4:
        {
            cmd ->cmd_type=4;
            cmd->p_hash_table=http_hash_person_list;
            dbug("message : user black list");
            break;
        }
        case 5:
        {
            cmd ->cmd_type=5;
            cmd->p_hash_table=http_hash_group_list;
            dbug("message : user group black list");
            break;
        }
        case 6:
        {
            cmd ->cmd_type=6;
            cmd->p_hash_table=http_hash_whole_list;
            dbug("message : all black list");
            break;
        }
        case 7:
        {
            dbug("message : url");
            break;
        }
        case 8:
        {

            dbug("message : url group");
            break;
        }
        case 9:
        {
            dbug("message : filetype");
            break;
        }
        default :
        {
            dbug("imp message error");
        }
        }

    }
    else
    {
         switch(buff[4]-'0')
        {
        case 3:
        {
            dbug("message : white list");
            cmd ->cmd_type=3;
            cmd->p_hash_table=tcp_hash_white_list;
                break;
        }
        case 4:
        {
            cmd ->cmd_type=4;
            cmd->p_hash_table=tcp_hash_person_list;
            dbug("message : user black list");
            break;
        }
        case 5:
        {
            cmd ->cmd_type=3;
            //  cmd->p_hash_table=
            dbug("message : user group black list");
            break;
        }
        case 6:
        {
            cmd ->cmd_type=3;
            //cmd->p_hash_table=
            dbug("message : all black list");
            break;
        }
        case 7:
        {
            dbug("message : url");
            break;
        }
        case 8:
        {

            dbug("message : url group");
            break;
        }
        case 9:
        {
            dbug("message : filetype");
            break;
        }
        default :
        {
            dbug("imp message error");
        }
        }
    }

    return buff[4]-'0';
}

int message_del()///如果是删除MES，就调用
{

    return 0;
}

int get_arg(char *buff)///通过ARG的个数，获取每一个参数
{

    return 0;
}


int message_add(char *buff)///如果是增加MES，就调用
{
    int mesg_type=0;
    struct cmd_info cmd;
    ///
    mesg_type=message_type(buff,&cmd);
    if(mesg_type>9)
    {
        dbug("wrong mesg_type");
        return 0;
    }
    else
    {
        dbug("mesg_type is %d ",mesg_type);
        if(cmd.cmd_type==1) return 0;
        else
        {
            dbug("process this add packet!!");
        }
    }

    return 0;
}





int process_message(char * buff)
{
    if(buff[0]=='A')
    {
        dbug("add mesg  %s",buff);
        message_add(buff);
    }
    if(buff[0]=='D')
    {
        dbug("del mesg  %s",buff);
    }
    return 0;
}

******************************************************************************/

int sock_server_cmd( )
{
    signal(SIGPIPE,SIG_IGN);
    int portno=6788;///YS 监听PHP那里的，用6788
    char buff[1024];
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n,flag=1,flag_len=sizeof(int);
    sockfd_cmd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_cmd < 0)
        dbug("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(setsockopt(sockfd_cmd,SOL_SOCKET,SO_REUSEADDR,&flag,flag_len)<0)
    {
        dbug("ERROR ON setockopt");
    }
    if (bind(sockfd_cmd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        dbug("ERROR on binding  2222");
    }

    listen(sockfd_cmd,5);
    while(1)
    {
        clilen = sizeof(cli_addr);
        newsockfd_cmd = accept(sockfd_cmd,     (struct sockaddr *) &cli_addr,  &clilen);
        if (newsockfd_cmd < 0)
        {
            dbug("ERROR on accept");
        }
        while(1)
        {
            n= read(newsockfd_cmd,buff,101);///YS 阻塞读

            if (n < 0)
            {
                dbug("ERROR read to socket");
                close(newsockfd_cmd);
                break;
            }
            else
            {
                printf("  %s %d \n",buff,n);
                char *ip =inet_ntoa(cli_addr.sin_addr);
	//	printf(" %s\n",ip);
	//	printf(" %s\n",db_ip);
//exit(0);
                if(!strcmp(ip,db_ip))
                {
                    dbug("cmd from correct ip");
                    if(!strcmp(buff,"reboot")) ///YS 如果是“REBOOT”就退出程序，我们的看门狗会启动他
                    {
                        dbug("imp reboot!!!");
                        if (newsockfd >0)///YS 如果审计端连接上了，就把数据向外发
                        {
                            char rbbuff[1544]={'\0'};
                            sprintf(rbbuff,"%s","reboot");
                            write(newsockfd,rbbuff,1544);///YS 发到审计端的数据

                        }
                        close(newsockfd_cmd);
                        close(sockfd_cmd);
                        exit(0);

                    }
                }
                else
                {
                    dbug("cmd from error ip");
                }
                //process_message(buff);
            }
        }
    }
    close(sockfd_cmd);
    return 0;
}


