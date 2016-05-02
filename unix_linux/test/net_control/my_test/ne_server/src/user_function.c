///YS 用户的操作的函数，主要有用户对应的组ID的查找
#include "public_function.h"
#include <glib.h>

GHashTable *user_hash_list;

extern void dbug(char *fmt,...);

/*****************************
该函数用于打印哈希表里的用户组和用户IP
参数hash_type，只用于打印输出，没实际意义
*****************************/
void user_hase_table_find(GHashTable* hash,int hash_type)
{

    dbug("######################## %d",hash_type);
    g_hash_table_foreach(hash, (GHFunc)public_fun_user_iterator, "The hash of %s        is          %s \n");

    dbug("######################## %d",hash_type);
}

/*****************************
该函数用于创建用户哈希表
*****************************/
void user_hase_table_create()
{
    user_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);

}

/*****************************
该函数用于插入用户信息到哈希表，该函数暂时没有使用
参数1“IP”，是用户的IP
参数2“GID”，是用户的组ID
*****************************/
void user_hase_table_insert(char *  ip,char * gid)
{
    g_hash_table_insert(user_hash_list, ip, gid);

}
