/*
 * test
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mysql.h>


struct device {
    int  dev_addr_code;
    char dev_name[128];
    char dev_loca[128];
    char dev_type[128];
    char dev_cmd[128];
    char alarm_cmd[128];
    char close_cmd[128];
    char dev_phone[128];
    char dev_email[128];

    int sms_type;                   //  0.温度报警  1.湿度报警   2.红外报警   3.烟雾报警
    int alarm_type;                 //  1.声光报警  2.SMS报警  3.Phone报警   4.E-Mail报警
    int alarm_flag;                 //  0.开关量报警；1.数值报警
    int switch_alarm_status;        //  开关量设备报警状态，默认为1
    int Min;
    int Max;
    int Min2;
    int Max2;
    char Time_Min[128];
    char Time_Max[128];
    int needAlarm;
    int isAlarm;
    int AlarmCount;
    int Alarm_Dev_Num;
};




/* MySQL database connect info */
#define MYSQL_HOST	"localhost"
#define MYSQL_USER	"dragonite"
#define	MYSQL_PASSWD	"dragonite"
#define MYSQL_DB	"meiguang"


/* Function prototypes */
void mysql_error_exit(const char *msg);
int mysql_connect();
void print_device_data(struct device *dev);


/* Global MYSQL structure */
MYSQL mysql;
int dev_count = 0;
struct device dev[100];


int
main(int argc, char *argv[])
{
	mysql_connect();

	read_database_setting();

	
	int i;
	char buff[128];
	for (i = 0; i < dev_count; ++i) {
		printf("dev_cmd = %s\n", dev[i].dev_cmd);
		string_2_hex(dev[i].dev_cmd, buff);
		//printf("buff: %x\n", buff);

		//
		if (strcmp(dev[i].dev_type , "UPS") == 0) {
			int t;
			for(t = 0; t < 30; ++t)
				printf("%x | ",buff[t]);
			printf("\n");
		}
	}
	


	/* Closes a previously opened connection */
	mysql_close(&mysql);

	return 0;
}

/*
 *
 */
void
mysql_error_exit(const char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, mysql_error(&mysql));
	exit(EXIT_FAILURE);
}

/*
 *
 */
int
mysql_connect(void)
{
	MYSQL *connect_ptr;

	/* Prepares and initializes a MYSQL structure to be used
	   withmysql_real_connect() */
	connect_ptr = mysql_init(&mysql);
	if (connect_ptr == NULL)
		mysql_error_exit("mysql_init() failed");

	/* We can set extra connect options and affect behavior for a
	   connection using mysql_options() */

	/* Establish a connection to database server */
	connect_ptr = mysql_real_connect(&mysql, MYSQL_HOST, MYSQL_USER,
					 MYSQL_PASSWD, MYSQL_DB, 0, NULL, 0);
	if (connect_ptr == NULL)
		mysql_error_exit("mysql_real_connect() failed");

	printf("Connect to mysql success!\n");

	/* Sets the default character set for the current connection */
	mysql_set_character_set(&mysql, "utf8");

	return 0;
}



int
read_database_setting(void)
{
	if (mysql_query(&mysql, "SELECT device_command.*, device_limits.* FROM device_command, device_limits WHERE device_command.Dev_Num=device_limits.Dev_Num;"))
		mysql_error_exit("mysql_query() failed");

	MYSQL_RES *result = mysql_store_result(&mysql);
	if (result == NULL)
		mysql_error_exit("mysql_store_result() failed");

	MYSQL_ROW row;
	int i = 0;

	int num_row, num_col;
	num_row = mysql_num_rows(result);
	num_col = mysql_num_fields(result);
	printf("num_row = %d, num_col = %d\n", num_row, num_col);


    while ((row = mysql_fetch_row(result)))
    {
        int dev_num_tmp=atoi(row[1]);
	printf("dev_num_tmp = %d\n", dev_num_tmp);

        if(row[1])  dev[dev_num_tmp].dev_addr_code=atoi(row[1]);
        if(row[2])  snprintf(dev[dev_num_tmp].dev_type,sizeof(dev[dev_num_tmp].dev_type),"%s",row[2]);
        if(row[4])  snprintf(dev[dev_num_tmp].dev_cmd,sizeof(dev[dev_num_tmp].dev_cmd),"%s",row[4]);
        if(row[5])  dev[dev_num_tmp].alarm_type = atoi(row[5]);
        if(row[6])  snprintf(dev[dev_num_tmp].alarm_cmd,sizeof(dev[dev_num_tmp].alarm_cmd),"%s",row[6]);
        if(row[7])  snprintf(dev[dev_num_tmp].close_cmd,sizeof(dev[dev_num_tmp].close_cmd),"%s",row[7]);
        if(row[8])  snprintf(dev[dev_num_tmp].dev_phone,sizeof(dev[dev_num_tmp].dev_phone),"%s",row[8]);
        if(row[9])  dev[dev_num_tmp].sms_type = atoi(row[9]);
        if(row[10]) snprintf(dev[dev_num_tmp].dev_email,sizeof(dev[dev_num_tmp].dev_email),"%s",row[10]);

        if(row[14]) snprintf(dev[dev_num_tmp].dev_name,sizeof(dev[dev_num_tmp].dev_name),"%s",row[14]);
        if(row[15]) snprintf(dev[dev_num_tmp].dev_loca,sizeof(dev[dev_num_tmp].dev_loca),"%s",row[15]);
        if(row[16]) dev[dev_num_tmp].alarm_flag = atoi(row[16]);
        if(row[17]) dev[dev_num_tmp].switch_alarm_status = atoi(row[17]);
        if(row[18]) dev[dev_num_tmp].Min = atoi(row[18]);
        if(row[19]) dev[dev_num_tmp].Max = atoi(row[19]);
        if(row[20]) dev[dev_num_tmp].Min2 = atoi(row[20]);
        if(row[21]) dev[dev_num_tmp].Max2 = atoi(row[21]);
        if(row[22]) dev[dev_num_tmp].Min = atoi(row[22]);
        if(row[23]) dev[dev_num_tmp].Max = atoi(row[23]);
        if(row[24]) dev[dev_num_tmp].Min2 = atoi(row[24]);
        if(row[25]) dev[dev_num_tmp].Max2 = atoi(row[25]);
        if(row[26]) snprintf(dev[dev_num_tmp].Time_Min,sizeof(dev[dev_num_tmp].Time_Min),"%s",row[26]);
        if(row[27]) snprintf(dev[dev_num_tmp].Time_Max,sizeof(dev[dev_num_tmp].Time_Max),"%s",row[27]);
        if(row[28]) dev[dev_num_tmp].needAlarm = atoi(row[28]);
        if(row[29]) dev[dev_num_tmp].Alarm_Dev_Num = atoi(row[29]);
        dev[dev_num_tmp].isAlarm = 0;
        i++;


	printf("-----------------------------------------------------------\n");
	print_device_data(&dev[dev_num_tmp]);
	printf("-----------------------------------------------------------\n");
    }
    dev_count=i;


	printf("dev_count = %d\n", dev_count);


    mysql_free_result(result);



    return 0;
}


void
print_device_data(struct device *dev)
{
	printf("dev_addr_code = %d\n", dev->dev_addr_code);
	printf("dev_name = %s\n", dev->dev_name);
	printf("dev_loca = %s\n", dev->dev_loca);
	printf("dev_type = %s\n", dev->dev_type);
	printf("dev_cmd = %s\n", dev->dev_cmd);
	printf("alarm_cmd = %s\n", dev->alarm_cmd);
	printf("close_cmd = %s\n", dev->close_cmd);
	printf("dev_phone = %s\n", dev->dev_phone);
	printf("dev_email = %s\n", dev->dev_email);

	printf("sms_type = %d\n", dev->sms_type);
	printf("alarm_type = %d\n", dev->alarm_type);
	printf("alarm_flag = %d\n", dev->alarm_flag);
	printf("switch_alarm_status = %d\n", dev->switch_alarm_status);
	printf("Min = %d\n", dev->Min);
	printf("Max = %d\n", dev->Max);
	printf("Min2 = %d\n", dev->Min2);
	printf("Max2 = %d\n", dev->Max2);

	printf("Time_Min = %s\n", dev->Time_Min);
	printf("Time_Max = %s\n", dev->Time_Max);

	printf("needAlarm = %d\n", dev->needAlarm);
	printf("isAlarm = %d\n", dev->isAlarm);
	printf("AlarmCount = %d\n", dev->AlarmCount);
	printf("Alarm_Dev_Num = %d\n", dev->Alarm_Dev_Num);
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
        // %x Matches  an  unsigned hexadecimal integer
        // scanf read from stdin
        // sscanf read from str
        sscanf(temp_buff,"%x",&temp);
        new_buff[j]=temp;
        j++;

    }
    new_buff[j+1]='#';
    new_buff[j+2]='#';

    return 0;
}
