/*
 * gcc thread_pool.c -lpthread -o thread_pool
 */




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>


/*
 * 线程池里所有运行和等待的任务都是一个 thread_worker
 * 由于所有任务都在链表里，所以是一个链表结构
 */
struct thread_worker {
	/* 回调函数，任务运行时会调用此函数，注意也可声明成其它形式 */
	void *(*func_addr)(void *arg);
	/* 回调函数的参数 */
	void *arg;
	struct thread_worker *next;
};

/*
 *
 */
struct thread_pool {
	/* Mutex variable */
	pthread_mutex_t queue_lock;
	/* Condition variable */
	pthread_cond_t queue_ready;
	/* 链表结构，线程池中所有等待任务 */
	struct thread_worker *queue_head;
	struct thread_worker *queue_end;
	/* 是否销毁线程池 */
	int is_destroy;
	/* Thread IDs (for create thread) */
	pthread_t *tids;
	/* 线程池中允许的活动线程数目 */
	int max_thread_num;
	/* 当前等待队列的任务数目 */
	int current_queue_size;
};



int pool_add_worker(void *(*func_addr)(void *arg), void *arg);
static void *thread_routine(void *arg);


static struct thread_pool *pool = NULL;


/*
 *
 */
void
init_thread_pool(int max_thread_num)
{
	printf("\ninit_thread_pool() start\n");

	pool = (struct thread_pool *) malloc(sizeof(struct thread_pool));

	/* Dynamically initialize the mutex variable */
	pthread_mutex_init(&(pool->queue_lock), NULL);
	/* dynamically initialize the condition variable */
	pthread_cond_init(&(pool->queue_ready), NULL);

	pool->queue_head = NULL;
	pool->queue_end = NULL;

	pool->max_thread_num = max_thread_num;
	pool->current_queue_size = 0;

	/* Thread pool is not destroy yet */
	pool->is_destroy = 0;

	pool->tids = (pthread_t *) malloc(max_thread_num * sizeof(pthread_t));

	int i;
	for (i = 0; i < max_thread_num; ++i)
		pthread_create(&(pool->tids[i]), NULL, thread_routine, NULL);

	printf("init_thread_pool() end\n\n");
}

/*
 * 向线程池中加入任务
 */
int
pool_add_worker(void *(*func_addr)(void *arg), void *arg)
{
	//printf("pool_add_work() running %d time(s)\n", *((int *) arg));
	/* 构造一个新任务 */
	struct thread_worker *new_worker =
		(struct thread_worker *) malloc(sizeof(struct thread_worker));
	new_worker->func_addr = func_addr;
	new_worker->arg = arg;
	new_worker->next = NULL;

	pthread_mutex_lock(&(pool->queue_lock));
	/* 将任务加入到等待队列中 */
	struct thread_worker *member = pool->queue_head;
	if (member != NULL)
		/* Link to the new worker */
		pool->queue_end->next = new_worker;
	else
		pool->queue_head = new_worker;
	pool->queue_end = new_worker;
	assert(pool->queue_head != NULL);

	pool->current_queue_size++;
	pthread_mutex_unlock(&(pool->queue_lock));

	/* 好了，等待队列中有任务了，唤醒一个等待线程；
	   注意如果所有线程都在忙碌，这句没有任何作用 */
	pthread_cond_signal(&(pool->queue_ready));

	//printf("pool_add_work() running %d time(s) end\n", *((int *) arg));

	return 0;
}

/*
 * 销毁线程池，等待队列中的任务不会再被执行，但是正在运行的线程会一直把任务运行完后再退出
 */
int
pool_destroy(void)
{
	/* 防止两次调用 */
	if (pool->is_destroy)
		return -1;
	pool->is_destroy = 1;

	printf("\nstart destroy thread pool\n\n");

	/* 唤醒所有等待线程，线程池要销毁了 */
	pthread_cond_broadcast(&(pool->queue_ready));
	/* 阻塞等待线程退出，否则就成僵尸了 */
	int i;
	for (i = 0; i < pool->max_thread_num; ++i)
		pthread_join(pool->tids[i], NULL);
	free(pool->tids);

	/* 销毁等待队列 */
	struct thread_worker *head = NULL;
	while (pool->queue_head != NULL) {
		head = pool->queue_head;
		pool->queue_head = pool->queue_head->next;
		free(head);
	}

	/* 条件变量和互斥量也别忘了销毁 */
	pthread_mutex_destroy(&(pool->queue_lock));
	pthread_cond_destroy(&(pool->queue_ready));

	free(pool);
	/* 销毁后指针置空是个好习惯 */
	pool = NULL;

	return 0;
}

static void *
thread_routine(void *arg)
{
	printf("Starting thread %ld\n", (long) pthread_self());

	while (1) {
		pthread_mutex_lock(&(pool->queue_lock));
		/* 如果等待队列为0并且不销毁线程池，则处于阻塞状态; 注意
		   pthread_cond_wait 是一个原子操作，等待前会解锁，唤醒后会加锁 */
		while (pool->current_queue_size == 0 && !pool->is_destroy) {
			printf("Thread %ld is waiting\n",
				(long) pthread_self());
			pthread_cond_wait(&(pool->queue_ready),
						&(pool->queue_lock));
		}

		/* 线程池要销毁了 */
		if (pool->is_destroy) {
			/* 遇到 break, continue, return 等跳转语句
			   千万不要忘记先解锁 */
			pthread_mutex_unlock(&(pool->queue_lock));
			printf("Thread %ld will exit\n", (long) pthread_self());
			pthread_exit(NULL);
		}


		printf("Thread %ld is starting to work\n",
			(long) pthread_self());

		/* assert 是调试的好帮手 */
		assert(pool->current_queue_size != 0);
		assert(pool->queue_head != NULL);

		/* 等待队列长度减去1，并取出链表中的头元素 */
		pool->current_queue_size--;
		struct thread_worker *worker = pool->queue_head;
		pool->queue_head = worker->next;
		pthread_mutex_unlock(&(pool->queue_lock));

		/* 调用回调函数，执行任务 */
		(*(worker->func_addr))(worker->arg);
		free(worker);
		worker = NULL;
	}

	return NULL;
}

static void *
thread_func(void *arg)
{
	printf("Thread id is %ld, working on task %d\n\n",
		(long) pthread_self(), *((int *) arg));
	sleep (1);

	return NULL;
}

int
main(int argc, char *argv[])
{
	int NUM_THREAD = 100;
	int NUM_WORK = 10;

	/* 线程池中最多 xx 个活动线程 */
	init_thread_pool(NUM_THREAD);
	/* 连续向池中投入 xx 个任务 */
	int *working_num = (int *) malloc(sizeof(int) * NUM_WORK);

	int i;
	for (i = 0; i < NUM_WORK ; ++i) {
		working_num[i] = i;
		pool_add_worker(thread_func, &working_num[i]);
	}
	printf("\n");

	/* 等待所有任务完成 */
	//sleep (5);
	/* 销毁线程池 */
	pool_destroy();
	free(working_num);

	return 0;
}
