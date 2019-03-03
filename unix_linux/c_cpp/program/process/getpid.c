/*
 * File: getpid.c
 * -------------
 * This program get the pid and ppid.
 */

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("PID = %i\n", getpid());
    printf("PPID = %i\n", getppid());

    return 0;
}
