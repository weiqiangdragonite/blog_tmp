///YS 一些公用的函数都在这里
#include <string.h>
#include <pcap/pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include "packet_info.h"
#include "ringbuff.h"
#include <libnet.h>
#include <stdarg.h>
#include "config.h"
#include <mysql.h>
#include <pthread.h>
#include <glib.h>
#include <time.h>
#include "openssl/md5.h"

MYSQL my_mysql;
struct udphdr *udp_header;
extern CircularBuffer cb_http_b;
extern pthread_mutex_t *mutex_for_cb_http_b;
extern pthread_cond_t *notFull_http_b, *notEmpty_http_b;
extern GHashTable *tcp_app_tactics_hash_list;
extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern pthread_mutex_t *tcp_hash_mutex;
extern void mysql_end(MYSQL_RES *res,MYSQL *sock);
extern int rst_tcp3(struct packet_info *my_packet);
extern int get_file_name_temp(int a,int b);
extern pthread_mutex_t *mutex_for_app_ip;
extern int sql_start(MYSQL *mysql);
struct app_server_ip app_sip[1000];
int app_sip_num=0;
extern char sub_net[24];///ys 要监听的子网，192.168
extern char error_report_ip[32];
int i=0;
extern int ex;
extern int sx;
extern int debug_open;
time_t timep;

char dst_db_ip[32]= {0};
char localip[32]= {0};
char dst_db_name[64]= {0};
char dst_db_user_name[64]= {0};
char dst_db_password[64]= {0};
char public_temp_buff[512];
extern char filepath[512];



/*********************
获取本地配置文件内容
参数1，filename是配置文件的位置
*********************/
int get_config_local_file(char *filename)
{
    FILE *fp=NULL;
    char c[1024];
    int i=0;
   // fp=fopen("/root/Desktop/mg_pro_zj/my_workspace/bin/Debug/config","r");
   fp=fopen("/mg/config","r");
   // fp=fopen(filename,"r");
    while(fgets(c,sizeof(c),fp)!=NULL)
    {
        for(i=0; i<64; i++)
        {
            if(c[i]=='\n') c[i]='\0';
        }

        if(strstr(c,"localip:"))
        {
            sprintf(localip,"%s",c+8);
        }

        if(strstr(c,"dbip:"))
        {
            sprintf(dst_db_ip,"%s",c+5);
        }
        if(strstr(c,"dbname:"))
        {
            sprintf(dst_db_name,"%s",c+7);
        }
        if(strstr(c,"dbusername:"))
        {
            sprintf(dst_db_user_name,"%s",c+11);
        }
        if(strstr(c,"dbpassword:"))
        {
            sprintf(dst_db_password,"%s",c+11);
        }
        if(strstr(c,"error_report_ip:"))
        {
            sprintf(error_report_ip,"%s",c+16);
        }
    }
    printf(" local_ip: %s\n db_ip: %s\n db_name: %s\n dn_user: %s\n db_pwd: %s\n",localip,dst_db_ip,dst_db_name,dst_db_user_name,dst_db_password);
    fclose(fp);
return 0;
}




/*********************
调试信息函数
参数型式与Printf函数一样
*********************/
void dbug(char *fmt,...)
{
    if(debug_open)///YS 是否打开调试信息
    {
        va_list ap;
        char string[256];
        va_start(ap,fmt);
        vsprintf(string,fmt,ap);
        if(strstr(string,"imp"))///YS  imp == important ，带颜色输出
        {
            printf("\e[33mdebug info >>>>");
            printf(string+3);
            printf("\e[0m \n");
        }
        else
        {
            printf("debug info >>>>");
            printf(string);
            printf("\n");
            va_end(ap);
        }
    }
}



/*********************
哈希表输出KEY,VALUE用的函数，要配合hash_foreach使用
*********************/
int public_fun_user_iterator(gpointer key, gpointer value ,gpointer user_data)
{
    printf("%s\t>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\t%ld\n",(char*)key, *(long int*)value);
    //g_printf(user_data, key, value);
    return 0;
}


/**********************
* 查看是哪种类型的数据链路，并且确定其包头的偏移量，暂时没用
* 参数1“ pd”是打开网卡后的句柄
* 出错返回-1
**********************/
int public_fun_datalink_off(pcap_t *pd)
{
    int offset = -1;
    switch (pcap_datalink(pd))///YS LIBPCAPAPI
    {
    case DLT_EN10MB:
        offset = 14;
        break;
    case DLT_IEEE802:
        offset = 22;
        break;
    case DLT_FDDI:
        offset = 21;
        break;
#ifdef DLT_LOOP
    case DLT_LOOP:
#endif
    case DLT_NULL:
        offset = 4;
        break;
    default:
        dbug("unsupported datalink type\n");
        break;
    }
    return (offset);
}





/*********************
把字符串转换成16进制的函数，暂时没用
*********************/
char * public_fun_str_2_hex(char * str,char *patten)
{
    int i=0,j=0;
    char hex_str[4];
    long hex ;
    while(str[i])
    {

        if(str[i]=='\\')
        {
            hex_str[0]=str[i+1];
            hex_str[1]=str[i+2];
            hex_str[2]=str[i+3];
            hex_str[3]=str[i+4];
            hex = strtol(hex_str, NULL, 16);
            i=i+5;
            patten[j]='\\';
            patten[j+1]='\\';
            patten[j+2]=hex;
            j=j+3;

        }
        else
        {
            patten[j]=str[i];
            i++;
            j++;
        }
    }
    return 0;
}


/********************************
*  1，配置BPF的过滤规则
*  2，参数 handle是打开网卡后的句柄，filter_exp是BPF表达式，fp 是BPF表达式在编译成二进制后的指令的指针，net 是网卡掩码
*  3，出错返回-1
**********************************/

int public_fun_bpf_config(pcap_t *handle,char *filter_exp,struct bpf_program fp,bpf_u_int32 net)
{
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)///YS LIBPCAPAPI,编译过虑条件
    {
        fprintf(stderr, "Couldn't parse filter %s: %s", filter_exp, pcap_geterr(handle));
        return -1;
    }
    if (pcap_setfilter(handle, &fp) == -1)///YS LIBPCAPAPI，设置编译过虑后的条件
    {
        fprintf(stderr, "Couldn't install filter %s: %s", filter_exp, pcap_geterr(handle));
        return -1;
    }
    dbug("BPF configure success!");
    return 0;
}





/*
*  获取HTTP包的要匹配信息
*  参数packet，原始包；str_cmp，要匹配的信息；len 长度；
*
*/
/*char * http_get_info(char *packet,char * str_cmp,int len)
{
    static char http_temp_info[public_function_http_str_len];
    char *p="0";
    char *p2="0";
    memset(http_temp_info,0,public_function_http_str_len);
    p=strstr(packet,str_cmp);
    if(p==NULL)
    {
        //dbug("can't find the str\n");
        return 0;
    }
    if(str_cmp[0]=='R')
    {
        p2=strchr(p+len,'/');
        if(p2==NULL)
        {
            p2=strstr(p,"\r\n");
            if(p2==NULL) return 0;

        }
        if(p2==NULL) return 0;
    }
    else
    {
        p2=strstr(p,"\r\n");
        if(p2==NULL) return 0;
    }
    //dbug("?????????????????  %d ,%s ,%s\n",len,p,p2);
    strncpy(http_temp_info,p+len,(p2-p)-len);
    return http_temp_info;


}

*/

/*********************
获取哈希表里某个KEY的命中次数，暂时没用
参数1“GHashTable* hashtable”，传入的哈希表指针
参数2“char * str_cmp”，要找的KEY
*********************/
int get_hash_count(GHashTable* hashtable,char * str_cmp)
{
    long int *hit_count;
    long int b_num=0;
    pthread_mutex_lock(tcp_hash_mutex);


    if((hit_count= (long int* )(g_hash_table_lookup(hashtable, str_cmp))))
    {

        b_num=*hit_count;
        b_num++;
        dbug("================%s ==================> %ld",str_cmp,  hit_count);
        g_hash_table_insert(hashtable, g_strdup(str_cmp), (gpointer)b_num);
        return 1;

    }
    else
    {
        return 0;
    }
    pthread_mutex_unlock(tcp_hash_mutex);
}


/********************
把数据包里的IP，存入到相应的缓存表中，该函数在我们找到数据包的特征字后才调用的
参数1“struct packet_info * packet_tmp”，是我们自定义的数据包
*********************/
int public_fun_put_Sip_into_cache(struct packet_info * packet_tmp)
{
    //dbug("impmatch   %s ",packet_tmp->behavior_type.detail);
    //dbug("impYY_server_ip:  %s",packet_tmp->dst_ip);
    //dbug("impgid:   %s",packet_tmp->gid);
    //dbug("impIP:   %s",packet_tmp->src_ip);
    //ys put into hash ys//
    //white list-> person list->gid list -> ALL list
if(!strstr(packet_tmp->src_ip,sub_net)) return 0;
else{
    printf("tttttttttttttt   %s\n",packet_tmp->src_ip);
}

    char str_temp[256];
    char cache_str[public_function_cache_str_len];
    char result[16];
    char *result_p;
    time(&timep);
    for(i=0; i<4; i++)///YS 找4次的原因是因为我们有白名单，个人黑名单，组黑名单，全局黑名单
    {
        memset(str_temp,0,256);
        if(i==0)
        {
            strcat(str_temp,packet_tmp->behavior_type.detail);
            strcat(str_temp,packet_tmp->src_ip);
        }
        if(i==1)
        {
            strcat(str_temp,packet_tmp->behavior_type.detail);
            strcat(str_temp,packet_tmp->gid);
        }
        if(i==2)
        {
            strcat(str_temp,packet_tmp->behavior_type.detail);
        }
//            dbug("imp str_temp ------------------------is %s",str_temp);
        result_p=g_hash_table_lookup(tcp_app_tactics_hash_list,str_temp);///YS 找一下这个包究竟是在哪种策略的，是白名单，还是个人黑名单，组黑名单，全局黑名单

        if(result_p)///YS 如果找到了的话，我们就更新应更的数据表和哈希表
        {

            strcpy(result,result_p);
            //dbug("imp  result is %s",result);

            memset(cache_str,0,public_function_cache_str_len);
            if(!strcmp(result,"white_list"))
            {
                dbug("imp got it ,add into the white list hash");
                strcat(cache_str,packet_tmp->src_ip);
                strcat(cache_str,packet_tmp->dst_ip);
                //dbug("########  %s",packet_tmp->dst_ip);
                g_hash_table_insert(tcp_hash_white_list, g_strdup(cache_str), (gpointer)timep);
                break;
            }
            if(!strcmp(result,"person_list"))
            {

                dbug("imp got it ,add into the person list hash");
                strcat(cache_str,packet_tmp->src_ip);
                strcat(cache_str,packet_tmp->dst_ip);

                g_hash_table_insert(tcp_hash_person_list, g_strdup(cache_str), (gpointer)timep);
                break;
            }
            if(!strcmp(result,"group_list"))
            {

                dbug("imp got it ,add into the group list hash");
                strcat(cache_str,packet_tmp->gid);
                strcat(cache_str,packet_tmp->dst_ip);
                g_hash_table_insert(tcp_hash_group_list, g_strdup(cache_str),(gpointer) timep);

                break;
            }
            if(!strcmp(result,"whole_list"))
            {
                dbug("imp got it ,add into the whole list hash");
                g_hash_table_insert(tcp_hash_whole_list, g_strdup(packet_tmp->dst_ip), (gpointer)timep);
                break;
            }

        }
    }

 		pthread_mutex_lock(mutex_for_cb_http_a);
    snprintf(app_sip[app_sip_num].name,32,"%s",packet_tmp->behavior_type.detail);
    snprintf(app_sip[app_sip_num].ip,32,"%s",packet_tmp->dst_ip);
    app_sip_num++;
   // printf("rrrrrrrrrrrrrrrrrrrrrrrr      %d\n",app_sip_num);
 		pthread_mutex_unlock(mutex_for_cb_http_a);
    return 0;
}

/********************
把数据包里的服务器IP，存入到数据库中，该函数在我们找到数据包的特征字后才调用的
*********************/
int put_server_ip_into_db()
{
    MYSQL_RES *temp_res;
    MYSQL_ROW temp_row;
    char cache_str[public_function_cache_str_len];
    i=0;
    for(i=0; i<app_sip_num; i++)///YS 这个服务器的保存数据现在是1000长度
    {
        if(strlen(app_sip[i].ip)>3)
        {
            memset(cache_str,0,public_function_cache_str_len);
            snprintf(cache_str,sizeof(cache_str),"select Tactics_type from app_table where name = '%s';",app_sip[i].name);///YS 先查找是那一种类型的应用
            if(sx==1)
            {
                printf("sssssssss\n");
//                exit(0);
            }
            if(mysql_query(&my_mysql,cache_str))
            {
                // fprintf(stderr,"Query failed (%s)\n",mysql_error(&my_mysql));
            }
            temp_res=mysql_store_result(&my_mysql);
            temp_row=mysql_fetch_row(temp_res);
            memset(cache_str,0,public_function_cache_str_len);
            snprintf(cache_str,sizeof(cache_str),"INSERT INTO `%s` ( `Name`, `IP`) VALUES ('%s', '%s');",temp_row[0],app_sip[i].name,app_sip[i].ip);///YS 根据不同应用的结果，把服务器IP插入到不同的数据表里

            if(mysql_query(&my_mysql,cache_str))
            {
                fprintf(stderr,"Query failed (%s)\n",mysql_error(&my_mysql));
                mysql_free_result(temp_res);
            }
        }
    }
    app_sip_num=0;///YS,把数据都写到数据库后，可以清0了
return 0;
}

/***********************
*  获取包的有效信息
*  参数1“packet”，是原始包；
* 	参数2“my_packet”，是我们自己的包
*
*************************/
int public_fun_get_packet_info(const u_char * packet,struct packet_info  *my_packet)
{
    //struct packet_info my_packet;

    struct ether_header *eth_header;//ether header
    //struct ip * ip_header; //ip header
    struct tcphdr *tcp_header;// tcp header
    eth_header=(struct ether_header*)packet;  ///YS 获取双方的MAC
    memset(my_packet->src_mac, 0, sizeof(my_packet->src_mac));
    for(i=0; i<6; i++)
    {
        my_packet->src_mac[i]=eth_header->ether_shost[i];
        my_packet->dst_mac[i]=eth_header->ether_dhost[i];
    }

        /***************************tCP  info  **************************/
        tcp_header= (struct tcphdr *)(packet+ sizeof(struct ether_header) + sizeof(struct ip));
        my_packet->dst_port=ntohs(tcp_header->th_dport);///YS 获取TCP目标端口
        my_packet->src_port=ntohs(tcp_header->th_sport);///YS 获取TCP源端端口
        my_packet->win=ntohs(tcp_header->th_win);///YS 获取TCP窗口大小
        my_packet->ack=ntohl(tcp_header->th_ack);///YS 获取TCPACK
        my_packet->seq=ntohl(tcp_header->th_seq);///YS 获取TCPSEQ
        rst_tcp3(my_packet);///YS 拦截该数据包

    return 0;

}
