///ys 只要我这里拦截到数据包是UDP且端口大于3K的，那就ARP它，ARP会持继2分钟
#include <libnet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <net/ethernet.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "packet_info.h"


u_char mac[6]; ///ys 好像没用了
char mac_2_ip[32];///ys MAC 对应 的IP
char db_ip[24];///ys 被欺骗的目标IP
int ip1,ip2,ip3,ip4;///ys 以IP1.IP2.IP3.IP4这样的型式来拿IP字符串每个字段的值
struct arp_user user_list[255];///ys 需要ARP的用户
extern char my_device[64];///ys 监听网卡
//extern u_int8_t arp_mac[6];
extern  unsigned int arp_mac2[6];///ys 假网关mac
unsigned int arp_mac_wrong[6]= {0x11,0x11,0x11,0x11,0x11,0x11}; ///ys 假网关mac
extern char arp_source[32];///ys 假网关IP
char ex_ip[24];///ys VIPIP1，不会被拦截的IP，叫VIPIP
char ex_ip1[24];///ys VIPIP2
char ex_ip2[24];///ys VIPIP3
extern char sub_net[24];///ys 要监听的子网，192.168
extern int newsockfd;///YS reboot CMD flag

extern char localip[32];

/**********************
该函数用于把需求ARP的目标IP加入到我们的APR_USRR_LIST里面，参数1"const u_char *my_packet",是我们通过LIBPCAP的抓到的原始的包
**********************/

/*int get_arp_user_list_from_db(MYSQL *sock)
{
    return 0;
#define usre_flow_list "select User_IP,Mac from flow_statistics where Block=1"
    MYSQL_ROW row;
    MYSQL_RES *res;
    if(mysql_query(sock,usre_flow_list))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    while ((row = mysql_fetch_row(res)))///YS 读数据库查找到的结果
    {
        printf("aaaaa  >>>  %s   %s \n",row[0],row[1]);
        sscanf(row[0],"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS 获取他的MAC
        //if(strlen(user_list[ip4].ip)<3||user_list[ip4].count>33 && user_list[ip4].flag==0)
        if(user_list[ip4].flag==0)///YS 如果还没发过ARP到这个IP的话,就把这个IP加入到我们的ARP_USR_LIST里,并且发送次数为1,已经加入的标志为1
        {
            printf("insert arp_usr_list   %s    %d\n",row[0],ip4);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            user_list[ip4].from=4;
              sscanf(row[1],"%x:%x:%x:%x:%x:%x",
                      & user_list[ip4].mac[0],& user_list[ip4].mac[1],& user_list[ip4].mac[2],& user_list[ip4].mac[3]
                      ,& user_list[ip4].mac[4],& user_list[ip4].mac[5]);
        }
        sprintf(user_list[ip4].ip,"%s",row[0]);
    }
    mysql_free_result(res);
    return 0;
}


*/

/**********************
该函数用于把需求ARP的目标IP加入到我们的APR_USRR_LIST里面，参数1"const u_char *my_packet",是我们通过LIBPCAP的抓到的原始的包
**********************/
/*int creat_arp_user_list(const u_char *my_packet)
{
    int j;
    struct ip * ip_header;
    struct ether_header *eth_header;

    eth_header=(struct ether_header*)my_packet; //数据包的帧的头，包括双方的MAC也在这里面
    ip_header = (struct ip *)(my_packet + sizeof(struct ether_header)); //数据包的IP的头，包括双方的IP也在这里面

    if(strstr(inet_ntoa(ip_header->ip_src),sub_net)&&strstr(inet_ntoa(ip_header->ip_dst),sub_net)) return 0;///如果是内网通信，不管他
    if(strstr(inet_ntoa(ip_header->ip_src),"255.255")||strstr(inet_ntoa(ip_header->ip_dst),"255.255")) return 0;///如果是内网通信，不管他

    strcpy(mac_2_ip,inet_ntoa(ip_header->ip_src));///YS 获取目标IP

    if(strstr(mac_2_ip,sub_net))///YS 判断是不是我们的内网IP，是的话就获取他的MAC
    {
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS 获取他的MAC
        //if(strlen(user_list[ip4].ip)<3||user_list[ip4].count>33 && user_list[ip4].flag==0)
        if(user_list[ip4].flag==0)///YS 如果还没发过ARP到这个IP的话,就把这个IP加入到我们的ARP_USR_LIST里,并且发送次数为1,已经加入的标志为1
        {
            printf("insert arp_usr_list   %s    %d\n",mac_2_ip,ip4);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_shost[j];
                printf("%x|\n", user_list[ip4].mac[j]);
            }
        }
    }
    else
    {
        strcpy(mac_2_ip,inet_ntoa(ip_header->ip_dst));
        if(!strstr(mac_2_ip,sub_net)) return 0;
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
        if(strlen(user_list[ip4].ip)<3||user_list[ip4].count>33)
        {
            printf("insert arp_usr_list     %d\n",ip4);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_dhost[j];
                printf("%x|", user_list[ip4].mac[j]);
            }
        }
    }
    sprintf(user_list[ip4].ip,"%s",mac_2_ip);

    return 0;
}
*/

/**********************
该函数用于把需求ARP的目标IP加入到我们的APR_USRR_LIST里面，参数1"const u_char *my_packet",是我们通过LIBPCAP的抓到的原始的包
**********************/
int send_arp_user(const u_char *my_packet)
{
    int j;
    struct ip * ip_header;
    struct ether_header *eth_header;

    eth_header=(struct ether_header*)my_packet; //数据包的帧的头，包括双方的MAC也在这里面
    ip_header = (struct ip *)(my_packet + sizeof(struct ether_header)); //数据包的IP的头，包括双方的IP也在这里面

    if(strstr(inet_ntoa(ip_header->ip_src),sub_net)&&strstr(inet_ntoa(ip_header->ip_dst),sub_net)) return 0;///如果是内网通信，不管他
    if(strstr(inet_ntoa(ip_header->ip_src),"255.255")||strstr(inet_ntoa(ip_header->ip_dst),"255.255")) return 0;///如果是内网通信，不管他

    strcpy(mac_2_ip,inet_ntoa(ip_header->ip_src));///YS 获取目标IP

    if(strstr(mac_2_ip,sub_net))///YS 判断是不是我们的内网IP，是的话就获取他的MAC
    {
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);///YS 获取他的MAC
        if(user_list[ip4].count==0 )
       // if(user_list[ip4].flag==0)///YS 如果还没发过ARP到这个IP的话,就把这个IP加入到我们的ARP_USR_LIST里,并且发送次数为1,已经加入的标志为1
        {
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            sprintf(user_list[ip4].arp_packet,"%s","vvvv");
            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_shost[j];
                printf("%x|\n", user_list[ip4].mac[j]);
            }
        }
    }
    else
    {
        strcpy(mac_2_ip,inet_ntoa(ip_header->ip_dst));
        if(!strstr(mac_2_ip,sub_net)) return 0;
        sscanf(mac_2_ip,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
        if(user_list[ip4].count==0)
        {
             printf(" 111111111 %d\n",user_list[ip4].flag);
            user_list[ip4].count=1;
            user_list[ip4].flag=1;
            sprintf(user_list[ip4].arp_packet,"%s","vvvv");

            for(j=0; j<6; j++)
            {
                user_list[ip4].mac[j]=eth_header->ether_dhost[j];
                printf("%x|", user_list[ip4].mac[j]);
            }
        }

    }
    sprintf(user_list[ip4].ip,"%s",mac_2_ip);

    return 0;
}




/*******************
arp欺骗包发送函数,从apr_user_list里拿IP然后去欺骗，完了后就加1，直到发了33个包后，再看看对方有没有继续发过高端口的UDP包，有的话，继续
***********************/
void arp_spoofind( )
{


    int i=0;
    while(1)
    {
        ///YS 每5秒钟发一次ARP
        sleep(1);

        for(i=0; i<255; i++)
        {
            if(user_list[i].flag!=1)
            {
                continue;
            }


            if(!strcmp(user_list[i].ip,db_ip))///YS 如果是数据库IP，就不发
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,ex_ip1))///YS 如果是VIP，就不发
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,arp_source))///YS 如果是网关IP，就不发
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,ex_ip))///YS 如果是VIP，就不发
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,ex_ip2))///YS 如果是VIP，就不发
            {
                continue;
            }
            if(!strcmp(user_list[i].ip,localip))///YS 如果是自己
            {
                continue;
            }

            user_list[i].count++;

            if(user_list[i].count==2)
            {
                if (newsockfd >0)///YS 如果审计端连接上了，就把数据向外发
                {
                    char buff[1600];
                    memcpy(buff,&user_list[i],1544);
                    if (write(newsockfd,&user_list[i],1544)< 0)///YS 发到审计端的数据
                    {
                        close(newsockfd);
                        newsockfd=-1;
                    }
                    else
                    {
                        printf("pass  arp>>>  %c  %c  %c  %c  \n",buff[0],buff[1],buff[2],buff[3]);
                        printf("pass  arp>>>  %s  \n",user_list[i].arp_packet);
                        printf("ARP SPOOFIND SENT %s\n",user_list[i].ip);
                    }
                }
            }
            if(user_list[i].count>33 )
            {
                user_list[i].flag=0;
                user_list[i].count=0;
            }

        }

    }

}
