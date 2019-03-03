/*
 * File: time2.c
 * -------------
 * This program show the UTC time and local time.
 */

#include <stdio.h>
#include <time.h>

int main(void)
{
    struct tm *ptr;
    time_t timeptr;
    time(&timeptr);

    ptr = gmtime(&timeptr);
    
    printf("UTC time is:   %s", asctime(ptr));
    printf("Local time is: %s", ctime(&timeptr));

    return 0;
}
