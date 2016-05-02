#include <stdio.h>
#include <string.h>

int str_to_hex(char *str, char *str_hex);
int string_2_hex(char *buff, char *new_buff);

int
main(void)
{
	char *str = "abcd";
	char str_hex[100];

	//str_to_hex(str, str_hex);
	//printf("str = %s, hex = %s\n", str, str_hex);

	char buf_1[100] = "abcd";
	char buf_2[100];
	string_2_hex(buf_1, buf_2);
	printf("buf_1 = %s, buf_2 = %s\n", buf_1, buf_2);

	return 0;
}

int
str_to_hex(char *str, char *str_hex)
{
	return 0;
}


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
	printf("temp_buff[0] = %c, temp_buff[1] = %c\n",
		temp_buff[0], temp_buff[1]);

        sscanf(temp_buff,"%x",&temp);
	printf("temp = %i\n", temp);
        new_buff[j]=temp;
	printf("new_buff[] = %c\n", new_buff[j]);
        j++;

    }
    new_buff[j+1]='#';
    new_buff[j+2]='#';

    return 0;
}
