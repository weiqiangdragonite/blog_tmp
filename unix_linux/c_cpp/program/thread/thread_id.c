/*
 * File: thread_id.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_id.c -lpthread -o thread_id
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

    printf("process pid is %i\n", getpid());

    // wait thread
    pthread_join(id, NULL);

    return 0;
}

void *thread(void *arg)
{
    printf("thread is running.\n");
    printf("the thread id is %u.\n", (unsigned int) pthread_self());
    printf("the process pid is %i.\n", getpid());

    return (void  *) 0;
}
