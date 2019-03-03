/*
 * File: pipe_rw.c
 * ---------------
 * This program use pipe
 */

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    int pipe_fd[2];
    pid_t pid;

    char buffer[80];
    //char msg[] = "hello";
    //char msg2[] = " pipe";
    
    // create pipe
    if (pipe(pipe_fd) < 0) {
        perror("create pipe error");
        return 1;
    }

    // create child process
    pid = fork();
    if (pid < 0) {
        perror("fork error");
        return 2;
    } else if (pid == 0) {
        puts("child process.");

        // close parent wirte pipe
        close(pipe_fd[1]) ;

        // stop the child process and let the parent run to write datas
        sleep(3);

        // child process read datas
        read(pipe_fd[0], buffer, sizeof(buffer));
        printf("child read from pipe is: %s\n", buffer);

        // close child pipe
        close(pipe_fd[0]);

    } else {
        puts("parent process.");

        // close child read pipe
        close(pipe_fd[0]);

        //
        sleep(2);

        // parent process write datas
        write(pipe_fd[1], "hello", 5);
        //printf("parent write to pipe is: %s\n", msg);

       if (write(pipe_fd[1], " pipe", 5) > 0) {
           printf("Parent write to pipe.\n");
       }

        // close parent pipe
        close(pipe_fd[1]);
        sleep(3);

        // wait for child process finish
        waitpid(pid, NULL, 0);
    }

    return 0;
}
