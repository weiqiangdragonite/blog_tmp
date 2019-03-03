/*
 * File: str.c
 * -----------
 * This program pass a string to a function.
 */

#include <stdio.h>

void printMessage(char msg[]);

int main(void)
{
    char quote[] = "Cookies make you fat";
    printf("The address of quote is %p\n", &quote);
    printf("quote occupies %i bytes\n", sizeof(quote));
    printMessage(quote);

    return 0;
}

void printMessage(char msg[])
{
    printf("Message is: %s\n", msg);
    printf("msg occupies %i bytes\n", sizeof(msg));
    printf("The address of msg is %p\n", &msg);
    printf("The address in msg is %p\n", *&msg);
}
