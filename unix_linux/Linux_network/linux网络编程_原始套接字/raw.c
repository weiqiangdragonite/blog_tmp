/* =====================================================================================  
 *  http://blog.csdn.net/yadon_z/article/details/7983402
 *
 *       Filename:  raw.c  
 *  
 *    Description:  使用原始套接字发送TCP协议，并外带自己的数据。  
 *  
 *        Version:  1.0  
 *        Created:  2012年09月11日 10时39分00秒  
 *       Revision:    
 *       Compiler:  GCC   
 *  
 *         Author:  yadon.z  
 *  
 * =====================================================================================  
 */  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netinet/ip.h>  
#include <netinet/tcp.h>  
#include <arpa/inet.h>  

#define DATA "hello"  
#define PACKET_SIZE sizeof(struct iphdr) + sizeof(struct tcphdr) + sizeof(DATA)

/*--------------------------------------------------------- 
 Function Name : check_sum() 
   Descrypthon : 校验和计算，摘自UNP源码 
------------------------------------------------------------*/  
unsigned short check_sum(unsigned short *addr, int len)  
{  
    int nleft = len;  
    int sum = 0;  
    unsigned short *w = addr;  
    short answer = 0;  
    while (nleft > 1) {  
        sum += *w++;  
        nleft -=2;  
    }  
    if (nleft == 1) {  
        *(unsigned char *)(&answer) = *(unsigned char *)w;  
        sum += answer;  
    }  
    sum = (sum >> 16) + (sum & 0xffff);  
    sum += (sum >> 16);  
    answer = ~sum;  
    return answer;  
}  
/*--------------------------------------------------------- 
 Function Name : init_socket() 
   Descrypthon : 初始化socket，使用原始套接字 
------------------------------------------------------------*/  
int init_socket(int sockfd, struct sockaddr_in *target,
                const char *dst_addr, const char *dst_port)  
{  
    const int flag = 1;  
    target->sin_family = AF_INET;  
    target->sin_port = htons(atoi(dst_port));  
    if (inet_aton(dst_addr, &target->sin_addr) == 0) {  
        perror("inet_aton fail\n");  
        exit(-1);  
    }  
    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {  
        perror("error");  
        exit(-1);  
    }  
    if (setsockopt(sockfd,IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) < 0) {  
        perror("setsockopt fail \n");  
        exit(-1);  
    }  
    return sockfd;  
}  
/*--------------------------------------------------------------- 
 Function Name : buile_iphdr() 
   Descrypthon : 构建IP头部数据， 源地址使用伪随机地址 
-----------------------------------------------------------------*/  
void buile_iphdr(struct sockaddr_in *target, char *buffer)  
{  
    struct iphdr *ip = (struct iphdr *)(buffer);  
    ip->version = 4;  
    ip->ihl = 5;  
    ip->tos = 0;  
    ip->tot_len = htons(PACKET_SIZE);  
    ip->id = 0;  
    ip->frag_off = 0;  
    ip->ttl = 255;  
    ip->protocol = IPPROTO_TCP;  
    ip->check = 0;  
    ip->saddr = random();  
    ip->daddr = target->sin_addr.s_addr;  
      
    ip->check = check_sum((unsigned short *)ip, sizeof(struct iphdr) + sizeof(DATA)); 

    printf("checksum = %04X\n", ip->check);
}  
/*--------------------------------------------------------------- 
 Function Name : buile_tcphdr() 
   Descrypthon : 构建TCP头部信息，并加入一些自己的数据，然后进行 
                 校验计算。 
-----------------------------------------------------------------*/  
void buile_tcphdr(struct sockaddr_in *target, const char *src_port, char *buffer)  
{  
    struct tcphdr *tcp = (struct tcphdr *)(buffer);  
    tcp->source = htons(atoi(src_port));  
    tcp->dest = target->sin_port;  
    tcp->seq = random();  
    tcp->doff = 5;  
    tcp->syn = 1;  
    buffer += sizeof(struct tcphdr);  
    memcpy(buffer, DATA, sizeof(DATA));  
    tcp->check = check_sum((unsigned short *)tcp, sizeof(struct tcphdr) + sizeof(DATA));  

    printf("checksum = %04X\n", tcp->check);
}  
int main(int argc, const char *argv[])  
{  
    char *buffer;  
    char *buffer_head = NULL;  
    int sockfd = 0;  
    struct sockaddr_in *target;  
    if (argc != 4) {  
        printf("usage: destination addresss, destination port, source port \n");  
        exit(-1);  
    }  
    const char *dst_addr = argv[1];  
    const char *dst_port = argv[2];  
    const char *src_port = argv[3];  
    target = calloc(sizeof(struct sockaddr_in),1);  
    buffer = calloc(PACKET_SIZE, 1);  
    buffer_head = buffer;  
    sockfd = init_socket(sockfd, target, dst_addr, dst_port);  
    buile_iphdr(target, buffer);  
    buffer += sizeof(struct iphdr);  
    buile_tcphdr(target, src_port, buffer);  
    sendto(sockfd, buffer_head, PACKET_SIZE, 0,
                              (struct sockaddr *)target, sizeof(struct sockaddr_in));  
    free(buffer_head);  
    free(target);  
  
    return 0;  
}  