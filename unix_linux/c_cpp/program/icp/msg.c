/*
 * File: msg.c
 * -----------
 * This program use msg.
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
    key_t key = ftok(".", 1);
    printf("key = %x\n", key);

    // then open/create the file
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("create error");
        return 1;
    }

    // init msg_buf
    struct msg_buf msgbuf;
    msgbuf.mtype = getpid();
    strcpy(msgbuf.mtext, "hello msg!");

    // send msg
    int result = msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), IPC_NOWAIT);
    if (result == -1) {
        perror("send msg error");
        return 2;
    }

    // get the msg
    memset(&msgbuf, 0, sizeof(msgbuf));
    //
    printf("after clear, msg is %s\n", msgbuf.mtext);
    result = msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), getpid(), IPC_NOWAIT);
    if (result == -1) {
        perror("receive msg error");
        return 3;
    }

    printf("The msg is %s\n", msgbuf.mtext);


    return 0;
}
