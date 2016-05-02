///YS UDP抓包函数

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap/pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <libnet.h>
#include "public_function.h"
#include "config.h"
#include "packet_info.h"
#include "ringbuff.h"
#include <pthread.h>


extern  void dbug(char *fmt,...);
extern int creat_arp_user_list(const u_char *my_packet);
extern char my_device[64];
extern GError *error;
extern int newsockfd;
extern MYSQL mysql;
struct sock_buff my_upd_sock_buff;
extern char ex_ip[24];
extern char ex_ip1[24];
extern char ex_ip2[24];
extern char arp_source[32];
/**********************
*把捕获的Udp数据包写入ARP_USER_LIST里
*参数 arg 是回调函数传进来的
*pkthdr是数据包的捕获时间与包的大小、捕获长度
*packet是数据包的原始数据
**********************/
struct udphdr *udp;
void udp_putin_buff(u_char *arg, const struct pcap_pkthdr* pkthdr,const u_char * packet)
{
    udp=(struct udphdr*)(packet+ sizeof(struct ether_header) + sizeof(struct ip));
    if(ntohs(udp->uh_sport)>3800 ||  ntohs(udp->uh_dport)  > 3800)
    {
        send_arp_user(packet);
       // creat_arp_user_list(packet);///YS 如果其中哪个端口都>3800的话，那就直接把那个包里的IP，写入到ARP的欺骗列表中，这个3800应该要以写到数据库里的配置表里
    }

    return;
}

/**********************
*捕获的UDP数据包线程
***********************/
void  udp_pcap()
{
    usleep(1000);
    pcap_t *descr = NULL;
    struct bpf_program fp;
    char errbuf[1024];
    char udp_bpf[2048];
    memset(udp_bpf,0,2048);
    memset(errbuf,0,1024);
    snprintf(udp_bpf,sizeof(udp_bpf),"udp and (not port 137)and (not port 139)and (not port 67)\
             and (not port 68) and (not port 135)and (not port 53) and (not port 138)and (not port 161) and (not host %s )\
             and (not port 162)and (not port 445)and (not port 500)\
             and (not port 5355)and (not port 1900)and (not host %s )and (not host %s )and (not host %s )",arp_source,ex_ip,ex_ip1,ex_ip2);///YS UDP过虑条件
    descr = pcap_open_live(my_device,MAXBYTES2CAPTURE,1,10,errbuf);
    if(public_fun_bpf_config(descr,udp_bpf,fp,0)==-1)
    {
        dbug("bpf config failed!\n");
        exit(0);
    }
    pcap_loop(descr,-1,udp_putin_buff,NULL);//参数2，多少个包后就停止捕获
    pcap_close(descr);
}

