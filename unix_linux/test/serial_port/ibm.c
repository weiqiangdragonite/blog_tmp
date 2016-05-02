#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix标准函数定义*/
#include     <sys/types.h>  /**/
#include     <sys/stat.h>   /**/
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include <string.h>

#define FALSE 0
#define TRUE 1

/***@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void*/

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
	    B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300,
	    38400,  19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed)
{
  int   i;
  int   status;
  struct termios   Opt;
  tcgetattr(fd, &Opt);
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
   {
   	if  (speed == name_arr[i])
   	{
   	    tcflush(fd, TCIOFLUSH);
    	cfsetispeed(&Opt, speed_arr[i]);
    	cfsetospeed(&Opt, speed_arr[i]);
    	status = tcsetattr(fd, TCSANOW, &Opt);
    	if  (status != 0)
            perror("tcsetattr fd1");
     	return;
     	}
   tcflush(fd,TCIOFLUSH);
   }
}
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄*
*@param  databits 类型  int 数据位   取值 为 7 或者8*
*@param  stopbits 类型  int 停止位   取值为 1 或者2*
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
 if  ( tcgetattr( fd,&options)  !=  0)
  {
  	perror("SetupSerial 1");
  	return(FALSE);
  }
  options.c_cflag &= ~CSIZE;
  switch (databits) /*设置数据位数*/
  {
  	case 7:
  		options.c_cflag |= CS7;
  		break;
  	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr,"Unsupported data size\n");
		return (FALSE);
	}
  switch (parity)
  	{
  	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/ 
		options.c_iflag |= INPCK;             /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;     /* Enable parity */
		options.c_cflag &= ~PARODD;   /* 转换为偶效验*/  
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S':
	case 's':  /*as no parity*/
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported parity\n");
		return (FALSE);
		}
  /* 设置停止位*/   
  switch (stopbits)
  	{
  	case 1:
  		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported stop bits\n");
		return (FALSE);
	}
  /* Set input parity option */
  if (parity != 'n')
  		options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

  tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
  if (tcsetattr(fd,TCSANOW,&options) != 0)
  	{
  		perror("SetupSerial 3");
		return (FALSE);
	}
  return (TRUE);
 }
/**
*@breif 打开串口
*/
int OpenDev(char *Dev)
{
int	fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
	if (-1 == fd)
		{ /*设置数据位数*/
			perror("Can't Open Serial Port");
			return -1;
		}
	else
	return fd;

}




int
read_data(int fd, char *buffer, ssize_t size)
{
	int i;
	char ch[1];
	ssize_t num_bytes;

	i = 0;
	while (1) {
		if (i >= size)
			break;

		num_bytes = read(fd, ch, sizeof(char));
		if (num_bytes == -1)
			return -1;

		buffer[i++] = ch[0];
	}

	return i;
}


/*
获取指令长度
*/
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
	int n_ret;

    if(!strcmp(dev_type,"UPS"))
    {
		//printf("333ups 进入了sent_cmd函数了\n");
//        n_ret=write(fd, cmd, strlen_my(cmd)-1);    //向串口写数据
//        usleep(700000);
//       n_ret= read(fd, buff, 1024);                //读取串口数据

        memset(buff,0,sizeof(buff));
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
        return 0;
    }

}





/**
*@breif 	main()
*/
int main(int argc, char **argv)
{
	int fd;
	int nread;
	int i;
	char *cmd = "010300000002C40B##";
	unsigned char data[8];



	char *dev ="/dev/ttyS0";
	fd = OpenDev(dev);
	if (fd>0)
    set_speed(fd,9600);
	else
		{
		printf("Can't Open Serial Port!\n");
		exit(0);
		}
  if (set_Parity(fd,8,1,'N')== FALSE)
  {
    printf("Set Parity Error\n");
    exit(1);
  }

  unsigned char buff[1024];
  char buff2[64]={'\0'};
  string_2_hex(cmd, buff2);

  while(1)
  	{
		memset(buff, 0, 10);
		sent_cmd(fd, buff2, buff,"temp");

		for (i = 0; i < 10; ++i)
			printf("%02x  ", buff[i]);
		printf("\n");
		sleep(1);
  	}
    //close(fd);
    //exit(0);
}
