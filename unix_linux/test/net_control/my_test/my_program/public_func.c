/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <glib.h>



#include "packet_info.h"


char debug_open = 1;



extern GHashTable *user_hash_list;
extern GHashTable *port_feature_hash_list;///ys 暂时没用了
extern GHashTable *http_hash_white_list; ///ys http 白名单 ip+hosturl
extern GHashTable *http_hash_person_list;///ys http 个人黑名单 ip+hosturl
extern GHashTable *http_hash_group_list;///ys http 组黑名单 usergroup+hosturl
extern GHashTable *http_hash_whole_list;///ys http 全局黑名单 hosturl
extern GHashTable *http_hash_ALL_w_list;///ys htpp all white，选择ALL，其中一个URL分类整个全选，使用ALL ，白名单
extern GHashTable *http_hash_ALL_b_list;///ys htpp all black ，黑名单

/****for tcp port or pcre****/
extern GHashTable *tcp_hash_white_list;///ys TCP白名单 IP＋DST—IP
extern GHashTable *tcp_hash_person_list;///ys 个人白名单 IP＋DST—IP
extern GHashTable *tcp_hash_group_list;///ys gid + Dst_ip
extern GHashTable *tcp_hash_whole_list;///ys Dst_ip


GHashTable *tcp_app_tactics_hash_list;///ys 这个哈希表是用于查APP对应的过滤条件，KEY：YY－－－》VALUE：BLACKLIST
GHashTable *tcp_port_list;
GHashTable *qq_ip_list;


/*********************
调试信息函数
参数型式与Printf函数一样
*********************/
void
dbug(char *fmt, ...)
{
	///YS 是否打开调试信息
	if (debug_open) {
		va_list ap;
		char string[256];
		va_start(ap,fmt);
		vsprintf(string,fmt,ap);
		///YS  imp == important ，带颜色输出
		if (strstr(string, "imp")) {
			printf("\e[33mdebug info >>>>");
			printf(string+3);
			printf("\e[0m \n");
		} else {
			printf("debug info >>>>");
			printf(string);
			printf("\n");
			va_end(ap);
		}
	}
}



/*****************************
该函数用于创建用户哈希表
*****************************/
void
user_hase_table_create(void)
{
	user_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);
}


/*****************************
*创建feature哈希表
*
*******************************/
void
feature_hase_table_create(void)
{
	///ys 哈希类型为g_str_hash, g_str_equal
	port_feature_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);
}


/*************************
*创建http哈希表
*
*************************/
void
http_hase_table_create(void)
{
	http_hash_white_list =  g_hash_table_new(g_str_hash, g_str_equal);
	http_hash_person_list = g_hash_table_new(g_str_hash, g_str_equal);
	http_hash_group_list = g_hash_table_new(g_str_hash, g_str_equal);
	http_hash_whole_list = g_hash_table_new(g_str_hash, g_str_equal);
	http_hash_ALL_w_list= g_hash_table_new(g_str_hash, g_str_equal);
	http_hash_ALL_b_list= g_hash_table_new(g_str_hash, g_str_equal);
}

/***********************
*创建tcp_app_tactics哈希表，这个哈希表保存的KEY：TCP服务器IP+个人用户IP，VALUE：黑名单，白名单，全局黑，组黑
************************/
void
tcp_app_tactics_hase_table_create(void)
{
	tcp_app_tactics_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);
}


/***********************
*创建tcp—port哈希表（带固定端口的）
***********************/
void
tcp_port_hase_table_create(void)
{
	tcp_port_list =  g_hash_table_new(g_str_hash, g_int_equal);
}

/***********************
*创建tcp哈希表
***********************/
void
tcp_hase_table_create(void)
{
	tcp_hash_white_list =  g_hash_table_new(g_str_hash, g_int_equal);
	tcp_hash_person_list = g_hash_table_new(g_str_hash, g_int_equal);
	tcp_hash_group_list = g_hash_table_new(g_str_hash, g_int_equal);
	tcp_hash_whole_list = g_hash_table_new(g_str_hash, g_int_equal);
	qq_ip_list =  g_hash_table_new(g_str_hash, g_int_equal);///YS QQ的独立也给一个，毕竟QQ是一个大块头
}

void
tcp_app_tactics_hase_table_insert(char *key, char *value)
{
	//dbug("imp hash key is %s  value is %s",key,value);
	//dbug("tcp_app_tactics_hash_list   key ====> %s   value=== > %s\n",key,value);
	g_hash_table_insert(tcp_app_tactics_hash_list, g_strdup(key), value);
}




/**************************
当我们的哈希表的条目和哈希表销毁时，会调该函数
***************************/
void free_data(gpointer hash_data)
{
    g_free(hash_data);
    hash_data=NULL;
}


/*************************
把TCP流中的两个不同方向的流转换成临时文件，XXX_IN  XXXX_OUT,方便后面审计时对该文件进行处理
参数1，是要处理的流
参数2，是IN方向的流的长度
参数3，是OUT方向的流的长度
*************************/
int create_tmp_file( struct stream *s,int in_len,int out_len)
{

    if(1)
    {
        if(in_len)
        {

            char filename[64];
            memset(filename,0,sizeof(filename));
            snprintf(filename,sizeof(filename),"%s_in", s->stream_index);///YS 流的对应的临时文件
            FILE *stream_p_in=NULL;
            stream_p_in = fopen(filename,"a+");///YS 以追加的型式打开，没有的话就创建一个文件
            if(stream_p_in)
            {
                fwrite(s->istream,in_len,1,stream_p_in);///YS 写入文件
                fclose(stream_p_in);
            }
        }

    }
    if(2)
    {
        if(out_len>0)
        {
            char filename[64];
            memset(filename,0,sizeof(filename));
            snprintf(filename,sizeof(filename),"%s_out",s->stream_index);///YS 同上
            FILE *stream_p_out=NULL;
            stream_p_out = fopen(filename,"a+");
            if(stream_p_out)
            {
                fwrite(s->ostream,out_len,1,stream_p_out);
                fclose(stream_p_out);
            }
        }
    }
    return 0;
}






