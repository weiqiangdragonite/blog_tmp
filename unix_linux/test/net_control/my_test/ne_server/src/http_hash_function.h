#include <glib.h>
#ifndef HTTP_HASH_FUNCTION_H
#define HTTP_HASH_FUNCTION_H
/***************************
*查询http哈希表，
*参数1“GHashTable* hash”,hash是哈希表
*参数2"int hash_type"是哈希类型，1，2，3，4分别对应white   person   group whole
****************************/
extern void http_hase_table_find(GHashTable* hash,int hash_type);

#endif
