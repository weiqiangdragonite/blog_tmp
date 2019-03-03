/*
 * File: thread_string.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_string.c -lpthread -o thread_string
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void *thread(void *arg);

int main(void)
{
    int result = 0;

    // thread id
    pthread_t id;

    // thread argument
    char buf[] = "hello, world";

    // create thread
    result = pthread_create(&id, NULL, (void *) thread, (void *) &buf);
    if (result) {
        perror("create thread error");
        return 1;
    }

    // wait thread
    pthread_join(id, NULL);

    return 0;
}

void *thread(void *arg)
{
    puts("thread start!");

    char *buf;
    buf = (char *) arg;
    printf("buffer is %s\n", buf);

    return (void *) 0;
}
