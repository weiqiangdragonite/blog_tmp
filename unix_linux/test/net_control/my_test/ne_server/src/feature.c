///YS 特征字处理
#include <glib.h>
#include "config.h"

GHashTable *port_feature_hash_list;
extern void dbug(char *fmt,...);

/**************************
*查询feature哈希表
*参数1“GHashTable* hash”,hash是哈希表
*参数2“int hash_type",是哈希类型，1，2，3，4分别对应white   person   group whole
***************************/
void feature_hase_table_find(GHashTable* hash,int hash_type)
{
    dbug("######################## %d",hash_type);
    // g_hash_table_foreach(hash, (GHFunc)iterator_feature, "The hash of %s is %s  ");
    /*
        gboolean result = g_hash_table_lookup_extended(hash, "192.168.1.22", (gpointer*)key_ptr, (gpointer*)value_ptr);
     if (result)
      {
      g_printf("Found that the %s >>>>>>>>> %s", capital, state);
     }
    */
    dbug("######################## %d",hash_type);
}


/*****************************
*创建feature哈希表
*
*******************************/
void feature_hase_table_create()
{
    port_feature_hash_list =  g_hash_table_new(g_str_hash, g_str_equal);///ys 哈希类型为g_str_hash, g_str_equal
}

/**************************
*插入feature哈希表，暂时没用这函数
***************************/
void feature_hase_table_insert(char *  port_and_level,char * feature)
{
    dbug("##################################3");
    g_hash_table_insert(port_feature_hash_list, port_and_level, feature);

}
