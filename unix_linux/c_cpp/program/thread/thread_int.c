/*
 * File: thread_int.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_int.c -lpthread -o thread_int
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

    // thread argument
    int num = 11;
    void *arg = &num;
    // int *tmp = &num
    // void *arg = (void *) tmp;
    // if this will be safer?
    // or (void *) &num

    // create thread
    result = pthread_create(&id, NULL, (void *) thread, arg);
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
    int *num;
    num = (int *) arg;
    printf("thread parameter is %i\n", *num);

    return (void *) 0;
}
