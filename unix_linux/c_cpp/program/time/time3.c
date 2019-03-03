/*
 * File: time3.c
 * -------------
 * This program calculate the function use time.
 */

#include <stdio.h>
#include <sys/time.h>

void function();

int main(void)
{
    struct timeval start;
    struct timeval end;
    double use;

    // start calculate time
    gettimeofday(&start, NULL);
    function();
    // end time
    gettimeofday(&end, NULL);

    // calculate the use time, change to usec
    use = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    // change to sec
    use /= 1000000;

    printf("Used time: %f sec.\n", use);

    return 0;
}

void function()
{
    double num = 0;
    for (unsigned int i = 0; i < 1000; ++i) {
        for (unsigned int j = 0; j < 1000; ++j) {
            ++num;
        }
    }
}
