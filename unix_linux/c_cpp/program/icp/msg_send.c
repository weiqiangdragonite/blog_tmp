/*
 * File: msg_send.c
 * -----------
 * This program use msg send data.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg_buf {
    int mtype;
    char mtext[80];
};

int main(void)
{
    // first get the key
    // set the pathname to current dtrectory
    key_t key = ftok("/home/dragonite/test", 1);
    printf("key = %x\n", key);

    // then open/create the file
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("create error");
        return 1;
    }

    printf("the msg id is %i\n", msgid);

    // init msg_buf
    struct msg_buf msgbuf;
    msgbuf.mtype = 1;
    strcpy(msgbuf.mtext, "hello world!");

    // send msg
    int result = msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), IPC_NOWAIT);
    if (result == -1) {
        perror("send msg error");
        return 2;
    }

    printf("The msg '%s' is send!\n", msgbuf.mtext);
    //sleep(20);

    return 0;
}
