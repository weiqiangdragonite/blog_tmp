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


char dst_db_name[64]={'\0'};
char dst_db_password[64]={'\0'};
char dst_db_user_name[64]={'\0'};
/*********************
获取本地配置文件内容
参数1，filename是配置文件的位置
*********************/
int get_config_local_file(char *filename)
{
    FILE *fp=NULL;
    char c[1024];
    int i=0;
   fp=fopen("/mg/self_config","r");
    while(fgets(c,sizeof(c),fp)!=NULL)
    {
        for(i=0; i<64; i++)
        {
            if(c[i]=='\n') c[i]='\0';
        }

        if(strstr(c,"dbname:"))
        {
            sprintf(dst_db_name,"%s",c+7);
        }
        if(strstr(c,"dbusername:"))
        {
            sprintf(dst_db_user_name,"%s",c+11);
        }
        if(strstr(c,"dbpassword:"))
        {
            sprintf(dst_db_password,"%s",c+11);
        }

    }
    fclose(fp);
return 0;
}

int sql_start(MYSQL *mysql)
{
	get_config_local_file("a");
    mysql_library_init(0,NULL,NULL);
    mysql_init(mysql);
    if (!(mysql_real_connect(mysql,"localhost",dst_db_user_name,dst_db_password,dst_db_name,0,NULL,CLIENT_MULTI_STATEMENTS)))
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

