#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <mysql.h>
#include "unistd.h"
#include "sys/types.h"
#include "sys/time.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "netdb.h"
#include "fcntl.h"
#include "errno.h"

#define clean_flow "update flow_statistics set flow =0 where User_IP != '' "
int sql_start(MYSQL *mysql)
{
    mysql_library_init(0,NULL,NULL);
    mysql_init(mysql);
    if (!(mysql_real_connect(mysql,"localhost","root","","mydb",0,NULL,CLIENT_MULTI_STATEMENTS)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s",mysql_error(mysql));
        perror("");
        mysql_close(mysql);
        exit(1);
    }
    else
    {
        mysql_set_character_set(mysql,"utf8");
        return 1;
    }
}
int main(int argc, char** argv)
{
    MYSQL mysql;
    sql_start(&mysql);
	if(mysql_query(&mysql,clean_flow))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(&mysql));
        exit(1);
    }
    
    return 0;
}

