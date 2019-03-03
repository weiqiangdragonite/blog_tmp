/*
 * File: pointer.c
 */

#include <stdio.h>

// global variable
int y = 1;

int main(void)
{
    int x = 4;
    printf("x is stored at %p\n", &x);
    
    printf("y is stored at %p\n", &y);

    return 0;
}
