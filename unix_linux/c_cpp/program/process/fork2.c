/*
 * File: fork2.c
 * -------------
 * This program try to vfork a child process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    int count = 0;

    pid = vfork();
    count++;

    if (pid < 0) {
        perror("Error in fork");
        exit(1);
    } else if (pid == 0) {
        printf("this is child process, id = %i\n", getpid());
        printf("count = %i\n", count);
        exit(0);
    } else {
        printf("this is parent process, id = %i\n", getpid());
        printf("count = %i\n", count);
    }

    return 0;
}
