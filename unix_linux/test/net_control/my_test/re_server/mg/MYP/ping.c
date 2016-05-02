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

char PIDFILE[256] = "/tmp/proc_sle_sav.pid";

static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return (fcntl(fd,cmd, &lock));
}

#define write_lock(fd, offset, whence, len) lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)

int check_single_on()
{
    int fd, val;
    char buf[10];

    if((fd = open(PIDFILE, O_WRONLY | O_CREAT, 0644)) <0 ) {
        perror("open in check_single_on()");
        exit(1);
    }

    /* try and set a write lock on the entire file */
    if(write_lock(fd, 0, SEEK_SET, 0) <0 ) {
        if(errno == EACCES || errno == EAGAIN) {
            //printf("daemon is already running.\n");
            //exit(0);            /* gracefully exit, daemon is already running */
            return 1;
        }else {
            perror("write_lock");
            close(fd);
            exit(1);
        }
    }

    /* truncate to zero length, now that we have the lock */
    if(ftruncate(fd, 0) <0) {
        perror("ftruncate");
        close(fd);      
        exit(1);
    }

    /* write our process id */
    sprintf(buf, "%d\n", getpid());
    if(write(fd, buf, strlen(buf)) != strlen(buf)) {
        perror("write in check_single_on()");
        close(fd);      
        exit(1);
    }

    /* set close-on-exec flag for descriptor */
    if((val = fcntl(fd, F_GETFD, 0) <0 )) {
        perror("fcntl");
        close(fd);
        exit(1);
    }
    val |= FD_CLOEXEC;
    if(fcntl(fd, F_SETFD, val) <0 ) {
        perror("fcntl again");
        close(fd);
        exit(1);
    }

    /* leave file open until we terminate: lock will be held */
    return 0;
}




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
(!(mysql_real_connect(mysql,"localhost","root","123456","mydb",0,NULL,CLIENT_MULTI_STATEMENTS)))
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
if(check_single_on())
{
printf("already running\n");
exit(0);
}


    MYSQL mysql;
    sql_start(&mysql);
    sql_get_tcp_port_to_hash(sql_str,&mysql);

    int i;
FILE *stream;
char buff[64];
while(1){
	sleep(1);
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

