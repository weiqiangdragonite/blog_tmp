/*
 * File: msg_receive.c
 * -------------------
 * This program use msg to receive data.
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
    int msgid = msgget(key, IPC_NOWAIT);
    if (msgid == -1) {
        perror("open error");
        return 1;
    }
    
    printf("the msg id is %i\n", msgid);

    // init msg_buf
    struct msg_buf msgbuf;

    // get the msg
    int result = msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), 1, IPC_NOWAIT);
    if (result == -1) {
        perror("receive msg error");
        return 3;
    }

    printf("The msg is %s\n", msgbuf.mtext);


    return 0;
}
