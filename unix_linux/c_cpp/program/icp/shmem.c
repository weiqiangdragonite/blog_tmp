/*
 * File: shmem.c
 * -------------
 * This program use share memory.
 */

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(void)
{
    int shmid;
    char *p_addr, *c_addr;

    // create share memory
    if ((shmid = shmget(IPC_PRIVATE, 1024, 0755)) == -1) {
        perror("Create share memory error");
        return 1;
    }

    //
    char buffer[] = "Hello World!";

    // create child process
    pid_t pid = fork();
    if (pid > 0) {
        puts("parent process!");

        // get thr share memory address
        p_addr = shmat(shmid, 0, 0);
        // init share memory
        memset(p_addr, '\0', 1024);
        // copy str to share memory
        strcpy(p_addr, buffer);
        printf("parent process write to share memory: %s\n", p_addr);
        //
        wait(NULL);

        // delete from share memory
        shmdt(p_addr);
    } else {
        puts("child process!");

        // prevent child process first run
        sleep(1);
        // get the share memory
        c_addr = shmat(shmid, 0, 0);
        printf("child process read from share memory: %s\n", c_addr);

        // delete from share memory
        shmdt(c_addr);
    }

    return 0;
}
