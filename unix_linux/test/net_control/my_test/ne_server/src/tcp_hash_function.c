//2-22
#include <glib.h>
#include "config.h"
#include <stdio.h>

GHashTable *tcp_hash_white_list;
GHashTable *tcp_hash_person_list;
GHashTable *tcp_hash_group_list;
GHashTable *tcp_hash_whole_list;
GHashTable *qq_ip_list;

GHashTable *tcp_app_tactics_hash_list;
GHashTable *tcp_port_list;

extern  void dbug(char *fmt,...);




/********************************
*查询tcp哈希表，
*参数：hash是哈希表
*tcp_hash_type是哈希类型，1，2，3，4----white   person   group whole
*********************************/
void tcp_hase_table_find(GHashTable* hash,int hash_type)
{
    dbug("######################## %d",hash_type);
//    printf("The xxxx is %s", (char *)g_hash_table_lookup(hash, "192.168.1.101183.61.143.43"));
    // g_hash_table_foreach(hash, (GHFunc)iterator_tcp, "The hash of %s is %s  ");
    /*
        gboolean result = g_hash_table_lookup_extended(hash, "192.168.1.22", (gpointer*)key_ptr, (gpointer*)value_ptr);
     if (result)
      {
      g_printf("Found that the %s >>>>>>>>> %s", capital, state);
     }
    */
    dbug("######################## %d",hash_type);
}


/***********************
*创建tcp哈希表
***********************/
void tcp_hase_table_create()
{
    tcp_hash_white_list =  g_hash_table_new(g_str_hash, g_int_equal);
    tcp_hash_person_list = g_hash_table_new(g_str_hash, g_int_equal);
    tcp_hash_group_list = g_hash_table_new(g_str_hash, g_int_equal);
    tcp_hash_whole_list = g_hash_table_new(g_str_hash, g_int_equal);
    qq_ip_list =  g_hash_table_new(g_str_hash, g_int_equal);///YS QQ的独立也给一个，毕竟QQ是一个大块头

}


/***********************
*创建tcp—port哈希表（带固定端口的）
***********************/
void tcp_port_hase_table_create()
{
    tcp_port_list =  g_hash_table_new(g_str_hash, g_int_equal);
}

/***********************
*创建tcp_app_tactics哈希表，这个哈希表保存的KEY：TCP服务器IP+个人用户IP，VALUE：黑名单，白名单，全局黑，组黑
************************/
void tcp_app_tactics_hase_table_create()
{
    tcp_app_tactics_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);
}


/******************************
*TCP应用的哈希表，保存了每个策略里的IP+IP，IP+UID的操作累型（是放行，还是拦截）
*1--->white(allow) ,0 ---->black(ban)
*YY+SIP||1
*YY+SIP||0
*YY+GID||0
*YY    ||0
********************************/
void tcp_app_tactics_hase_table_insert(char *  key,char * value)
{
    //dbug("imp hash key is %s  value is %s",key,value);
    dbug("tcp_app_tactics_hash_list   key ====> %s   value=== > %s\n",key,value);
    g_hash_table_insert(tcp_app_tactics_hash_list, g_strdup(key), value);
}



/*
*插入tcp应用哈希表，保存的是IP，IP（KEY，跟VALUE都是IP），暂时没有使用到
*tcp_hash_type是哈希类型，1，2，3，4----white   person   group whole
*/
void tcp_hase_table_insert_nouse(char *  ip_address,char * ip,int tcp_type)
{

    switch(tcp_type)
    {
    case white_list:
        g_hash_table_insert(tcp_hash_white_list, ip_address, ip);
        break;
    case person_list:
        g_hash_table_insert(tcp_hash_person_list, ip_address, ip);
        break;
    case group_list:
        g_hash_table_insert(tcp_hash_group_list, ip_address, ip);
        break;
    case whole_list:
        g_hash_table_insert(tcp_hash_whole_list, ip_address, ip);
        break;
    default :
        break;
    }

}
