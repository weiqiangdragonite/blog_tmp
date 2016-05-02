#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include "packet_info.h"
#include <time.h>
#include "config.h"
#include <mysql/mysql.h>

extern GHashTable *tcp_stream_list;
extern int create_tmp_file( struct stream * s,int in_len,int out_len);
extern int process_stream(struct stream * s);
extern int flow_count_insert_db();
time_t time_now;
extern int sockfd;
long int sum_temp=0;
char date[24];
int n;
char *buff2[1024];
static long packet_numss;
extern int sx;
extern MYSQL mysql;
/***********************
这个函数主要看看哪个TCP流的时间超时了，超时的就拿去审计，没超时的，证明还在使用，数据还没有传完，这里可能会有个BUG，如果人家下载10G的东西呢？
***********************/
void get_time(gpointer key, gpointer value ,gpointer user_data)
{

    struct stream *s=NULL;
    if(value) s=(struct stream *)value;
    if(s)
    {
    	printf("111111111 %s\n",s->stream_index);
        if(time_now-s->time>=10)///YS 现在的话，是超时了10秒，都没有收到数据包的话，就拿出来审计了
        {
            int in_count=s->in_effect_len-s->in_buff_record;
            int out_count=s->out_effect_len-s->out_buff_record;
            if(in_count>0||out_count>0)
            {

                dbug("time out  %s  inlen %d  ineffectlen %ld \n",s->stream_index,in_count,s->in_effect_len);
                create_tmp_file(s, in_count,out_count);///YS 先创建流文件的临时文件
                process_stream(s);///YS 处理流
            }

           if( g_hash_table_remove(tcp_stream_list,(key)))///YS 处理完了，就把数据给清了
           {

               dbug("%s g_hashtable remove sucess!!!\n",s->stream_index);
           }

        }
    }
}



/***********************
遍历整个哈希表
参数1，是需要被遍历的哈希表
***********************/
int foreach_hash(GHashTable *hash)
{
    time(&time_now);
    struct tm *tm_now;
    tm_now=localtime(&time_now);
    strftime(date,sizeof(date),"%Y-%m-%d",tm_now);///YS 这个TIME是我用作生成随机数的基数

    g_hash_table_foreach(hash, (GHFunc)get_time, NULL);///YS g_hash_table_foreach，GLIBAPI，会执行函数get_time
    dbug("after hash_foreach hash count is %d\n",g_hash_table_size(tcp_stream_list));
       return 0;
}



/************************
定时时间到了，会执行该函数
************************/
void sigFunc()
{
    printf("1111 p num is %ld\n",packet_numss);
    flow_count_insert_db();///YS 把每个用户的流量放入到数据库
    if(tcp_stream_list) foreach_hash(tcp_stream_list);///YS 把超时的TCP流挑出来，进行审计

	printf("1111 p num is %ld\n",packet_numss);
    packet_numss=packet_numss+35;
    if(packet_numss>60*60*3|| packet_numss==35)
    {
       report_error2();
       packet_numss++;
    }
    get_arp_user_list_from_db(&mysql);
    if(sx==1) {
		//printf("in sigFunc  \n");
		//exit(0);
		}
}


/************************
定时任务，该任务的功能是把超时的TCP流挑出来，进行审计。
************************/
int cache_count(void)
{
    printf("1111 p num is %ld\n",packet_numss);
    struct itimerval tv, otv;
    signal(SIGALRM, sigFunc);///YS 时间到了后，会运行该函数sigFunc
    tv.it_value.tv_sec = 2;
    tv.it_value.tv_usec = 0;
    tv.it_interval.tv_sec = 35; ///YS 以后每隔35秒可以运行一次，这个值应该写到数据库的配置表里
    tv.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &tv, &otv) != 0)
        dbug("setitimer err %d\n", errno);
return 0;
}

