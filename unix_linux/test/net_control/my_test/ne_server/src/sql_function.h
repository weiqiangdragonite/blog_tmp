#ifndef SQL_FUNCTION_H
#define SQL_FUNCTION_H
#include <mysql.h>
#include <glib.h>
extern void mysql_end(MYSQL_RES *res,MYSQL *sock);

extern int sql_get_website_to_hash(char *SELECT_QUERY,MYSQL *sock,int http_type,GHashTable * hash_table);

extern int sql_start(MYSQL *mysql);

#endif
