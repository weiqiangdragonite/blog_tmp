/*
 * File: thread_share.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_share.c -lpthread -o thread_share
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int num = 1;

void *thread(void *arg);

int main(void)
{
    int result = 0;

    // thread id
    pthread_t id;

    // create thread
    result = pthread_create(&id, NULL, (void *) thread, NULL);
    if (result) {
        perror("create thread error");
        return 1;
    }

    // run thread
    pthread_join(id, NULL);

    return 0;
}

void *thread(void *arg)
{
    printf("thread can get the global num %i\n", num);

    return (void  *) 0;
}
