/*
 * File: thread_create.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_create.c -lpthread -o thread_create
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread1(void);
void *thread2(void);

int main(void)
{
    int result = 0;

    // thread id
    pthread_t id1, id2;

    // create thread 1
    result = pthread_create(&id1, NULL, (void *) thread1, NULL);
    if (result) {
        perror("create thread 1 error");
        return 1;
    }

    // create thread 2
    result = pthread_create(&id2, NULL, (void *) thread2, NULL);
    if (result) {
        perror("create thread 2 error");
        return 2;
    }

    // wait thread
    //pthread_join(id1, NULL);
    //pthread_join(id2, NULL);

    for (int i = 0; i < 10; ++i) {
        printf("this is parent process, num = %i\n", i);
        sleep(1);
    }

    return 0;
}

void *thread1(void)
{
    for (int i = 0; i < 10; ++i) {
        printf("this is thread 1, num = %i\n", i);
        sleep(2);
    }
}

void *thread2(void)
{
    for (int i = 0; i < 10; ++i) {
        printf("this is thread 2, num = %i\n", i);
        sleep(1);
    }
}
