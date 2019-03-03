/*
 * File: thread_struct.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_struct.c -lpthread -o thread_struct
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct buffer {
    int id;
    char name[80];
};

void *thread(void *arg);

int main(void)
{
    int result = 0;

    // thread id
    pthread_t id;

    // thread argument
    struct buffer buf;
    buf.id = 1;
    strcpy(buf.name, "hello, world!");
    //void *arg = &buf;

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

    struct buffer *buf;
    buf = (struct buffer *) arg;
    printf("buffer id is %i\n", buf->id);
    printf("buffer name is %s\n", buf->name);

    return (void *) 0;
}
