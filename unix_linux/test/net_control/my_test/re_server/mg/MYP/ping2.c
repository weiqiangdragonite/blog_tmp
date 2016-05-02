#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <mysql/mysql.h>
#include "unistd.h"
#include "sys/types.h"
#include "sys/time.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "netdb.h"
#include "fcntl.h"
#include "errno.h"

#define sql_str "select User_IP from user;"
char straddr[500][32];
void usage()
{
    printf("use format:\n\t-s startip\n\t-e endip\n\t-t connect_timeout\n\t-p port\n");
    exit(-1);
}
int sql_start(MYSQL *mysql)
{
    mysql_init(mysql);
    mysql_thread_init();

    if 
(!(mysql_real_connect(mysql,"localhost","root","b2c123456","mydb",0,NULL,CLIENT_MULTI_STATEMENTS)))
    {
        fprintf(stderr,"Couldn't connect to engine!\n%s",mysql_error(mysql));
        perror("");
        exit(1);
    }
    else
    {
        mysql_set_character_set(mysql,"utf8");
        return 1;
    }
}

int sql_get_tcp_port_to_hash(char *SELECT_QUERY,MYSQL *sock)
{
    MYSQL_ROW row;
    MYSQL_RES *res;

    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    if (!(res=mysql_store_result(sock)))
    {
        fprintf(stderr,"Couldn't get result from %s", mysql_error(sock));
        exit(1);
    }
    int i=0;
    while ((row = mysql_fetch_row(res)))
    {
        sprintf(straddr[i],"%s",row[0]);
        printf(" >>>  %s\n",straddr[i]);
        i++;
    }
    mysql_free_result(res);

    return 0;
}


int insert_into_db(char *ip,MYSQL *sock,int online)
{
    MYSQL_ROW row;
char SELECT_QUERY[1024];
if(online)
sprintf(SELECT_QUERY,"update user set Online = 1 where User_IP= '%s';",ip);
else
sprintf(SELECT_QUERY,"update user set Online = 0 where User_IP= '%s';",ip);
    if(mysql_query(sock,SELECT_QUERY))
    {
        fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
        exit(1);
    }

    return 0;
}

int main(int argc, char** argv)
{
	char buffer[1024*40]={'\0'};
	FILE * fp;
	int flag_running=0;
	fp=popen("ps -aux | grep \"MYP\"","r");
	while(!feof(fp))
	{
		fgets(buffer,sizeof(buffer),fp);
		if(strstr(buffer,"MYP"))
		{
		printf("%s\n",buffer);
			++flag_running;
			if(flag_running>2)
			{
				printf("!!!!!!! already running\n");
				exit(0);
			}
		}
	}




    MYSQL mysql;
    sql_start(&mysql);
    sql_get_tcp_port_to_hash(sql_str,&mysql);

    int i;
FILE *stream;
char buff[64];
while(1){
	sleep(3);
	/*read_ret=read(fd,buff,6);
	if(read_ret>5) 
	{
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  %d   %s\n",read_ret,buff);
	system("pkill rebuild_packet");
      stream = fopen("/home/www/reboot.txt","w+");
	fclose(stream);
	}*/
    for( i=0; i<500; i++)
    {
		
        if(strlen(straddr[i])<5) continue;
		sleep(2);
        int ret=0;
        char str_tmp[64];
        sprintf(str_tmp,"ping -t 1 -c 1 %s",straddr[i]);
        ret=system(str_tmp);
        if(ret==0)
        {
            insert_into_db(straddr[i],&mysql,1);
        }
        else
        {
            insert_into_db(straddr[i],&mysql,0);
        }
    }
}
    return 0;
}

