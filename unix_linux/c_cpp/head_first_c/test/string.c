/*
 * File: string.c
 */

#include <stdio.h>

int main(void)
{
    char str[] = "Hello";
    printf("%s\n", str);

    str[0] = 'X';
    printf("%s\n", str);

    char str1 = 'a';
    printf("%c\n", str1);

    char str2 = 'b';
    printf("%c\n", str2);

    return 0;
}
