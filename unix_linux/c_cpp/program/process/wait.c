/*
 * File: wait.c
 * -------------
 * This program wait a process.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child, child_pid;
    child = fork();

    // now have two process running
    if (child < 0) {
        perror("Error in fork");
    } else if (child == 0) {
        // child process
        printf("I am the child process, ID = %i\n", getpid());
        sleep(10);
        printf("sleep over\n");
    } else {
        // parent process
        printf("I am the parent process, ID = %i\n", getpid());
        child_pid = wait(NULL);
        printf("I catched a child process with pid = %i\n", child_pid);
    }

    return 0;
}
