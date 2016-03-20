/*
 * 使用消息队列发送消息
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>

#define BUF_SIZE 1024

const key_t KEY = 999;

struct my_msg_st {
	long int my_msg_type;
	char some_text[BUF_SIZE];
};


int
main(int argc, char *argv[])
{
	int running = 1;
	int msgid;
	struct my_msg_st some_data;
	char buf[BUF_SIZE];

	/* 1. create msg queue */
	msgid = msgget(KEY, 0666 | IPC_CREAT);
	if (msgid == -1) {
		perror("msgget() failed");
		exit(EXIT_FAILURE);
	}

	/* 2. send msg */
	while (running) {
		printf("enter some text: ");
		fgets(buf, sizeof(buf), stdin);

		some_data.my_msg_type = 1;
		strncpy(some_data.some_text, buf, BUF_SIZE);

		if (msgsnd(msgid, (void *) &some_data, BUF_SIZE, 0) == -1) {
			perror("msgsnd() failed");
			exit(EXIT_FAILURE);
		}

		if (strncmp(buf, "end", 3) == 0)
			running = 0;
	}

	return 0;
}



