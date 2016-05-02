#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termio.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bits/signum.h>
#include <sys/resource.h>

#define COM4 "/dev/ttyS0"

int fd, fd2,fd_SMS,fd_UPS, frw, fd_controller, fd_ups;
struct termios oldtio, newtio;
int n_ret=0;


/*
串口设置函数
*/
//serial port set function
void setTermios(struct termios *pNewtio, unsigned short uBaudRate)
{
    bzero(pNewtio,sizeof(struct termios));
    pNewtio->c_cflag = uBaudRate|CS8|CREAD|CLOCAL;
    pNewtio->c_iflag = IGNPAR;
    pNewtio->c_oflag = 0;
    pNewtio->c_lflag = 0;
    pNewtio->c_cc[VINTR] = 0;
    pNewtio->c_cc[VQUIT] = 0;
    pNewtio->c_cc[VERASE] = 0;
    pNewtio->c_cc[VKILL] = 0;
    pNewtio->c_cc[VEOF] = 4;
    pNewtio->c_cc[VTIME] = 5;
    pNewtio->c_cc[VMIN] = 0;
    pNewtio->c_cc[VSWTC] = 0;
    pNewtio->c_cc[VSTART] = 0;
    pNewtio->c_cc[VSTOP] = 0;
    pNewtio->c_cc[VSUSP] = 0;
    pNewtio->c_cc[VEOL] = 0;
    pNewtio->c_cc[VREPRINT] = 0;
    pNewtio->c_cc[VDISCARD] = 0;
    pNewtio->c_cc[VWERASE] = 0;
    pNewtio->c_cc[VLNEXT] = 0;
    pNewtio->c_cc[VEOL2] = 0;
}

/*
打开串口COM4
*/

int COM4_init()
{
    if ((fd = open(COM4, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) //open serial COM4
    {
        return -1;
    }
    tcgetattr(fd, &oldtio);
    setTermios(&newtio, B2400);     //设置串口比特率
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);
    return 0;
}


/*
打开串口COM4
*/

int COM4_init2()
{
    if ((fd2 = open(COM4, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) //open serial COM4
    {
        printf("打开串口失败！\n");
        return -1;
    }
    tcgetattr(fd2, &oldtio);
    setTermios(&newtio, B9600);     //设置串口比特率
    tcflush(fd2, TCIFLUSH);
    tcsetattr(fd2, TCSANOW, &newtio);
    return 0;
}

int strlen_my(char *buff)
{
    int i=0;
    for(i=0; i<1024; i++)
    {
        if(buff[i]=='#'&& buff[i+1]=='#')
        {
            break;
        }
        else
        {
            continue;
        }
    }
    return i;
}

/*
字符串转换为16进制
*/
int string_2_hex(char *buff,char *new_buff)
{
    int temp;
    char temp_buff[2];
    int i=0,j=0;
    for (i=0; i<strlen(buff); i=i+2)
    {
        if(buff[i]=='#') break;

        temp_buff[0]=buff[i];
        temp_buff[1]=buff[i+1];
        sscanf(temp_buff,"%x",&temp);
        new_buff[j]=temp;
        j++;

    }
    new_buff[j+1]='#';
    new_buff[j+2]='#';

    return 0;
}



/*
向串口写数据并读取串口返回的数据
*/
int sent_cmd(int fd, char *cmd,  char *buff,char *dev_type)
{
	memset(buff,0,sizeof(buff));

    if(!strcmp(dev_type,"UPS"))
    {
		//printf("333ups 进入了sent_cmd函数了\n");
//        n_ret=write(fd, cmd, strlen_my(cmd)-1);    //向串口写数据
//        usleep(700000);
//       n_ret= read(fd, buff, 1024);                //读取串口数据

        n_ret=write(fd, cmd, strlen_my(cmd));    //向串口写数据
		//printf("444ups 命令写入到串口了，返回的值是 %d，这个值不能是负数，可不可以为0我也不太清楚\n",n_ret);
        // sleep(1);
        usleep(700000);
        //printf("33333333 %d\n",fd);
        n_ret=read(fd, buff, 1024);                //读取串口数据
		//printf("555ups 写完命令到串口了，返回的值是 %d，这个值不能是负数，可不可以为0我不太清楚\n",n_ret);
		//printf("666ups 写完命令到串口了，我们拿到的结果是（以16进制显示）：\n");
//		int t=0;
//		for(t=0;t<30;t++)
//		{
//			printf("%x|",buff[t]);
//		}
		//printf("\n你可以自己比较一下这个结果是不是你想要的\n");
        return 0;
    }
    else
    {
        n_ret=write(fd, cmd, strlen_my(cmd));    //向串口写数据
        usleep(700000);                    //休眠0.5秒
        n_ret=read(fd, buff, 333);              //读取串口数据
	printf("n_ret = %d\n", n_ret);
        return 0;
    }

}


int
main(void)
{
	// fd2
	COM4_init2();

	//char buff[1024];
	//char buff2[64]={'\0'};
	//char *dev_cmd = "010300000002C40B##";
	//char *dev_type = "temp";

        //string_2_hex(dev_cmd, buff2);
	//sent_cmd(fd2, buff2, buff, dev_type);

	ssize_t num_bytes;
	unsigned char cmd[8];
	unsigned char data[20];
	cmd[0] = 0x01;
	cmd[1] = 0x03;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x02;
	cmd[6] = 0xC4;
	cmd[7] = 0x0B;

	num_bytes = write(fd2, cmd, sizeof(cmd));
	if (num_bytes != sizeof(cmd)) {
		fprintf(stderr, "write() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("write %d bytes\n", (int) num_bytes);

	usleep(100000);
	num_bytes = read(fd2, data, sizeof(data));
	if (num_bytes == -1) {
		fprintf(stderr, "read failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("read %d bytes\n", (int) num_bytes);

	int i;
	for (i = 0; i < num_bytes; ++i)
		printf("data[%d] = 0x%02x\n", i, (unsigned char) data[i]);
}
