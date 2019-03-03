/*
 * File: execlp.c
 * -------------
 * This program use execlp.
 */

#include <unistd.h>

int main(void)
{
    execlp("ls", "ls", "-al", "/etc/passwd", NULL);
    return 0;
}
