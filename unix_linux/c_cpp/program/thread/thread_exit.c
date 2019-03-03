/*
 * File: thread_exit.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_exit.c -lpthread -o thread_exit
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

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

    // wait thread
    void *tmp;
    pthread_join(id, &tmp);
    printf("thread exit code is: %s\n", (char *) tmp);

    return 0;
}

void *thread(void *arg)
{
    printf("thread 1 running\n");

    //return (void *) 1;
    pthread_exit("thread is over");
}
