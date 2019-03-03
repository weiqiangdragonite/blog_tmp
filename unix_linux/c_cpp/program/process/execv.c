/*
 * File: execv.c
 * -------------
 * This program use execv.
 */

#include <unistd.h>

int main(void)
{
    char *argv[] = { "ls", "-al", "/etc/passwd", '\0' };
    execv("/bin/ls", argv);
    return 0;
}
