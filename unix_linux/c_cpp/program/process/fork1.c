/*
 * File: fork1.c
 * -------------
 * This program try to fork a child process.
 */

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    pid = fork();

    /*
    // now have two process running
    if (pid < 0) {
        perror("Error in fork");
    } else if (pid == 0) {
        // child process
        printf("I am the child process, ID = %i\n", getpid());
    } else {
        // parent process
        printf("I am the parent process, ID = %i\n", getppid());
    }
    */

    if (pid < 0) {
        perror("Error in fork");
        return 0;
    }

    while (1) {
        if (pid == 0) {
            printf("I am the child process, ID = %i\n", getpid());
            sleep(1);
        } else {
            printf("I am the parent process, ID = %i\n", getppid());
            sleep(2);
        }
    }

    return 0;
}
