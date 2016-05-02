///YS  cache 方式：本来是以“命中次数”来算的，现在的做法是改成了
///YS  以“最近访问的时间”来算，越是经常访问的IP，就放到内存的缓存表里，不经常访问的，在保存到数据库后，会被清出缓存表。
///YS  下面注释中，请以“最近访问的时间”来看待。

#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include "public_function.h"
#include <glib.h>
#include "config.h"
#include <stdio.h>
#include "packet_info.h"
#include <time.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include "openssl/md5.h"

MYSQL cache_mysql;
MYSQL_RES *cache_res;
MYSQL_ROW cache_row;
char key_p[32];///YS 临时的全局变量，用于保存缓存表的KEY，实际是IP
long int value_p;///YS 临时的全局变量，用于保存缓存表的VALUE，实际是更新时间
long int sum_temp=0;///YS 临时的全局变量，用于记录各个缓存表里有多少条记录
char CACHE_SELECT_QUERY[cache_sql_str_len];///YS 临时的全局变量，用于保存要执行的SQL语句

extern GHashTable *tcp_hash_white_list;
extern GHashTable *tcp_hash_person_list;
extern GHashTable *tcp_hash_group_list;
extern GHashTable *tcp_hash_whole_list;
extern int put_server_ip_into_db();
extern int cache_to_db(char * file_name);
extern char *get_cache_num (char *filename,long *len,char **buffer);
extern void dbug(char *fmt,...);
extern int sql_start(MYSQL *mysql);
extern int cache_count_time ;
extern long packet_num;
extern int get_file_name_temp(int a,int b);
extern int cache_num_limit ;
char filepath[512];
/****************************
该函数是把每一条记录都写入到数据库
参数1“char *table_name”，是表名
参数2“char * key”，是哈希表的KEY，在这代码里是IP
参数3“long int value”是哈希表的VALUE，在代码里是命中次数
*****************************/
int input_into_mysql( char *table_name,char * key,long int value)
{
    memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
    snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"select valuetime from `%s` where keyip = '%s';",table_name,key);

    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }

    cache_res=mysql_store_result(&cache_mysql);

    if(mysql_num_rows(cache_res))///YS 是INSERT，还是UPDATE
    {
        ///ys  update
        memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
        //sprintf(CACHE_SELECT_QUERY,"1111");
        snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"UPDATE `%s` SET `valuetime`='%ld' WHERE `keyip`='%s';",table_name,value,key);

    }
    else
    {
        ///ys insert
        memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
        snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"INSERT INTO `%s` (`keyip`, `valuetime`) VALUES ('%s', '%ld');",table_name,key,value);
    }

    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }

    mysql_free_result(cache_res);

return 0;
}


/****************************
该函数是把每一条记录都写入到数据库，方便一会用数据库的功能进行排序就可以达到筛选访最多的IP
参数1“gpointer key”，是哈希表的KEY，在这代码里是IP
参数2“gpointer value”是哈希表的VALUE，在代码里是命中次数
参数3“gpointer user_data”是用户的数据，在代码里是哈希表对应的类型
*****************************/
int get_hit_count_persent(gpointer key, gpointer value ,gpointer user_data)
{
    sum_temp++;///YS 缓存表对应的条目总数++，用于判断是否大于我们设置的缓存表大小的值
    memset(key_p,0,32);
    strcpy(key_p,(char *)key);
    value_p=0;
    value_p=((long int ) value);///YS 获取命中次数
    switch(*(int*)user_data)
    {
    case person_list :
    {
        //dbug("impputinto mysql ~~~~~~~~~~~~~~~~ person_list_cache");
        input_into_mysql("tcp_cache_list_person",key_p,value_p);///YS 把KEY,VALUE插入数据库
        break;
    }
    case group_list :
    {
        //dbug("impputinto mysql ~~~~~~~~~~~~~~~~group_list_cache");
        input_into_mysql("tcp_cache_list_group",key_p,value_p);///YS 把KEY,VALUE插入数据库
        break;
    }
    case whole_list :
    {
        //dbug("impputinto mysql ~~~~~~~~~~~~~~~~whole_list_cache");
        input_into_mysql("tcp_cache_list_whole",key_p,value_p);///YS 把KEY,VALUE插入数据库
        break;
    }
    }

return 0;
}

/****************************
清表函数，把命中数低的删除
函数1“GHashTable* hash”，是要处理的哈希表
函数2“char *table_name”，是每个哈希表相应的表名，该表用于保存各缓存表里的KEY->VALUE
****************************/
int cache_del(GHashTable* hash,char *table_name)
{
    memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);///YS 清空我们的临时BUFF，该BUFF，用于保存SQL语句
    snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"select count(valuetime) from `%s` ;",table_name);///YS 算一下我们的表里有多少条记录
    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))///YS MYSQLAPI，执行我们的SQL
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }
    cache_res=mysql_store_result(&cache_mysql);///YS MYSQLAPI，获取结果
    cache_row=mysql_fetch_row(cache_res);

    /*???????????????????这上面的3句话好像没什么用???????????????????*/

    ///ys查询“cache_num_limit”个结果，并且这些结果是按时间的大小进行DESC排序的
    memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
    snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"SELECT keyip FROM `%s` order by `valuetime` desc LIMIT %d,%s;",table_name,cache_num_limit,cache_row[0]);

    if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
        //exit(1);
    }

    cache_res=mysql_store_result(&cache_mysql);
    while((cache_row=mysql_fetch_row(cache_res))) ///YS 一条条去删除
    {
        g_hash_table_remove(hash,cache_row[0]);///YS 把那些时间比较旧的条目从哈希表里删除

        memset(CACHE_SELECT_QUERY,0,cache_sql_str_len);
        snprintf(CACHE_SELECT_QUERY,sizeof(CACHE_SELECT_QUERY),"delete  FROM `%s` where `keyip` = '%s';",table_name,cache_row[0]);///YS 并且从数据库表里删除

        if(mysql_query(&cache_mysql,CACHE_SELECT_QUERY))
        {
            fprintf(stderr,"Query failed (%s)\n",mysql_error(&cache_mysql));
            //exit(1);
        }

    }
    mysql_free_result(cache_res);

return 0;
}
/***************************
遍历哈希表缓存表，并且把这里面最经常访问的IP拿出来
参数1“GHashTable* hash”是要保存的哈希表
参数2“int  p_hash_sum_Type”是哈希表对应的类型
****************************/
void cache_count_hash(GHashTable* hash,int  p_hash_sum_Type)
{
    dbug("cache running =======================");

    sum_temp=0;///YS 该变量保存哈希表里有多少条记录
    ///YS gilbAPI，遍历哈希表，每个KEY,VALUE都用函数“get_hit_count_persent"进行处理
    ///YS 在处理完成后，我们的数据库对应的表里会保存了所有的缓存表的数据，然后我们利用数据库的功能来排序过虑
    g_hash_table_foreach(hash, (GHFunc)get_hit_count_persent,&p_hash_sum_Type);

   ///YS 如果表里的记录数量大于我们限定的数量，也就是哈希表满了，
   ///YS 我们就去读相应的数据库的表，并进行排序，得出的结果再放回我们的哈希表中
    if(sum_temp>cache_num_limit)
    {
        dbug("the cache is full!!!");
        switch(p_hash_sum_Type)
        {
        case person_list :
        {
            cache_del(hash,"tcp_cache_list_person");///YS 清表处理
            break;
        }
        case group_list :
        {
            cache_del(hash,"tcp_cache_list_group");
            break;
        }
        case whole_list :
        {
            cache_del(hash,"tcp_cache_list_whole");
            break;
        }
        }


    }
    dbug("cache end ============================");
}

/***************************
定时时间到了，就会执行该函数，该函数现在主要是针对TCP拦截的，不是HTTP拦截的，
因为TCP的识别每次都要经过特征字的分析，这样太慢，直接把那个包的IP记录下来，
像YY协议的IP是XX.XX.XX.X那以后看到这个XXIP，就可以直接当成是YY了，
虽然XX->IP，这样不太准确，因为这个IP有可能跑其它的应用，不过可以接受。
*///////////////////////////
void sigFunc()
{
    packet_num=packet_num+cache_count_time;
    if(packet_num>60*60*3||packet_num==cache_count_time)
    {
        report_error();
        packet_num++;
    }
//        get_arp_user_list_from_db(&cache_mysql);
//  cache_count_hash(tcp_hash_white_list,white_list);
    cache_count_hash(tcp_hash_person_list,person_list);///YS 遍历个人黑名单
    cache_count_hash(tcp_hash_group_list,group_list);///YS 遍历组黑名单
    cache_count_hash(tcp_hash_whole_list,whole_list);///YS 遍历全局名单
	put_server_ip_into_db();///YS 遍历完了，就把每个缓存表里的经常访问的服务器IP保存到数据库的表里，以后重启代码时也可以重用这些IP
}


/****************************
定时任务,任务是更新我的缓存表(主要是HASH表),表的大小可以在数据库里设置,同时更新的时间也可以在数据库里设置
*****************************/
int cache_count(void)
{
    sql_start(&cache_mysql);
    struct itimerval tv, otv;
    signal(SIGALRM, sigFunc); ///ys 时间到了后，执行的函数是sigFunc

    tv.it_value.tv_sec = 4;///ys 程序启动4秒后跑第一次
    tv.it_value.tv_usec = 0;

    tv.it_interval.tv_sec = cache_count_time; ///ys第一次,过后,以后每隔cache_count_time秒多久跑一次，该值在数据库里
    tv.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &tv, &otv) != 0) ///YS 系统调用，设置定时
        printf("setitimer err %d\n", errno);


return 0;
}



