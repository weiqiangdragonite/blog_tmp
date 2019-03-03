/*
 * File: signal.c
 * --------------
 * This program use signal.
 */

#include <stdio.h>
#include <signal.h>

void func(int signal);

int main(void)
{
    puts("Waitting for signal SIGINT or SIGQUIT!");

    //
    signal(SIGINT, func);
    signal(SIGQUIT, func);
    signal(SIGBUS, func);

    pause();

    return 0;
}

void func(int signal)
{
    if (signal == SIGINT) {
        puts("I get SIGINT!");
    } else if (signal == SIGQUIT) {
        puts("I get SIGQUIT");
    } else if (signal == SIGBUS) {
        puts("I get SIGBUS");
    }
}
