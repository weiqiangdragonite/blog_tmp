/*
 * File: thread_clean.c
 * ---------------------
 * This program create thread.
 * compile: gcc -std=c99 thread_clean.c -lpthread -o thread_clean
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread1(void *arg);
void *thread2(void *arg);
void *clean(void *arg);

int main(void)
{
    int result = 0;

    // thread id
    pthread_t id1, id2;

    // create thread 1
    result = pthread_create(&id1, NULL, (void *) thread1, (void *) 1);
    if (result) {
        perror("create thread 1 error");
        return 1;
    }

    // create thread 2
    result = pthread_create(&id2, NULL, (void *) thread2, (void *) 1);
    if (result) {
        perror("create thread 2 error");
        return 2;
    }

    // wait thread
    void *tmp;
    pthread_join(id1, &tmp);
    printf("thread 1 exit code is: %i.\n", (int) tmp);

    pthread_join(id2, &tmp);
    printf("thread 2 exit code is: %i.\n", (int) tmp);

    return 0;
}

void *clean(void *arg)
{
    printf("cleanup: %s\n", (char *) arg);
    return (void *) 0;
}

void *thread1(void *arg)
{
    printf("thread 1 start!\n");

    pthread_cleanup_push((void *) clean, "thread 1 first handler");
    pthread_cleanup_push((void *) clean, "thread 1 second handler");
    printf("thread 1 push done!\n");

    if (arg) {
        return (void *) 11;
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    printf("thread 1 pop done!\n");

    return (void *) 1;
}

void *thread2(void *arg)
{
    printf("thread 2 start!\n");
    
    pthread_cleanup_push((void *) clean, "thread 2 first handler");
    pthread_cleanup_push((void *) clean, "thread 2 second handler");
    printf("thread 2 push done!\n");

    if (arg) {
        pthread_exit((void *) 2);
    }

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    printf("thread 2 pop done!\n");

    pthread_exit((void *) 22);

}
