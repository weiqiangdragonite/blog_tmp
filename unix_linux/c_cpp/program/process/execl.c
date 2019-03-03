/*
 * File: execl.c
 * -------------
 * This program use execl.
 */

#include <unistd.h>

int main(void)
{
    execl("/bin/ls", "ls", "-al", "/etc/passwd", NULL);
    return 0;
}
