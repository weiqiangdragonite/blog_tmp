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
���������Ҫ�����ĸ�TCP����ʱ�䳬ʱ�ˣ���ʱ�ľ���ȥ��ƣ�û��ʱ�ģ�֤������ʹ�ã����ݻ�û�д��꣬������ܻ��и�BUG������˼�����10G�Ķ����أ�
***********************/
void get_time(gpointer key, gpointer value ,gpointer user_data)
{

    struct stream *s=NULL;
    if(value) s=(struct stream *)value;
    if(s)
    {
    	printf("111111111 %s\n",s->stream_index);
        if(time_now-s->time>=10)///YS ���ڵĻ����ǳ�ʱ��10�룬��û���յ����ݰ��Ļ������ó��������
        {
            int in_count=s->in_effect_len-s->in_buff_record;
            int out_count=s->out_effect_len-s->out_buff_record;
            if(in_count>0||out_count>0)
            {

                dbug("time out  %s  inlen %d  ineffectlen %ld \n",s->stream_index,in_count,s->in_effect_len);
                create_tmp_file(s, in_count,out_count);///YS �ȴ������ļ�����ʱ�ļ�
                process_stream(s);///YS ������
            }

           if( g_hash_table_remove(tcp_stream_list,(key)))///YS �������ˣ��Ͱ����ݸ�����
           {

               dbug("%s g_hashtable remove sucess!!!\n",s->stream_index);
           }

        }
    }
}



/***********************
����������ϣ��
����1������Ҫ�������Ĺ�ϣ��
***********************/
int foreach_hash(GHashTable *hash)
{
    time(&time_now);
    struct tm *tm_now;
    tm_now=localtime(&time_now);
    strftime(date,sizeof(date),"%Y-%m-%d",tm_now);///YS ���TIME������������������Ļ���

    g_hash_table_foreach(hash, (GHFunc)get_time, NULL);///YS g_hash_table_foreach��GLIBAPI����ִ�к���get_time
    dbug("after hash_foreach hash count is %d\n",g_hash_table_size(tcp_stream_list));
       return 0;
}



/************************
��ʱʱ�䵽�ˣ���ִ�иú���
************************/
void sigFunc()
{
    printf("1111 p num is %ld\n",packet_numss);
    flow_count_insert_db();///YS ��ÿ���û����������뵽���ݿ�
    if(tcp_stream_list) foreach_hash(tcp_stream_list);///YS �ѳ�ʱ��TCP�����������������

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
��ʱ���񣬸�����Ĺ����ǰѳ�ʱ��TCP����������������ơ�
************************/
int cache_count(void)
{
    printf("1111 p num is %ld\n",packet_numss);
    struct itimerval tv, otv;
    signal(SIGALRM, sigFunc);///YS ʱ�䵽�˺󣬻����иú���sigFunc
    tv.it_value.tv_sec = 2;
    tv.it_value.tv_usec = 0;
    tv.it_interval.tv_sec = 35; ///YS �Ժ�ÿ��35���������һ�Σ����ֵӦ��д�����ݿ�����ñ���
    tv.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &tv, &otv) != 0)
        dbug("setitimer err %d\n", errno);
return 0;
}
