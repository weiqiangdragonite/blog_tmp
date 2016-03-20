/*
 * 使用消息队列接收消息
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
	long int msg_to_receive = 0;

	/* 1. create msg queue */
	msgid = msgget(KEY, 0666 | IPC_CREAT);
	if (msgid == -1) {
		perror("msgget() failed");
		exit(EXIT_FAILURE);
	}

	/* 2. get msg */
	while (running) {
		if (msgrcv(msgid, (void *) &some_data, BUF_SIZE,
		msg_to_receive, 0) == -1) {
			perror("msgrcv() failed");
			exit(EXIT_FAILURE);
		}

		printf("You wrote: %s", some_data.some_text);
		if (strncmp(some_data.some_text, "end", 3) == 0)
			running = 0;
	}

	/* delete */
	if (msgctl(msgid, IPC_RMID, 0) == -1) {
		perror("msgctl() failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}



