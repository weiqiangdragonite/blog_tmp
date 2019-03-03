/*
 * File: time1.c
 * -------------
 * This program get the UTC time and local time.
 */

#include <stdio.h>
#include <time.h>

int main(void)
{
    struct tm *ptr;
    time_t timeptr;
    time(&timeptr);

    ptr = gmtime(&timeptr);
    printf("The UTC time is:   %i-%i-%i %i:%i\n",
            1990 + ptr->tm_year, 1 + ptr->tm_mon, ptr->tm_mday,
            ptr->tm_hour, ptr->tm_min, ptr->tm_sec);

    ptr = localtime(&timeptr);
    printf("The local time is: %i-%i-%i %i:%i\n",
            1990 + ptr->tm_year, 1 + ptr->tm_mon, ptr->tm_mday,
            ptr->tm_hour, ptr->tm_min, ptr->tm_sec);

    return 0;
}
