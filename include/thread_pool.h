#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

#define TASK_QUEUE_SIZE 32
#define THREADS_MAX 50

typedef struct {
    pthread_mutex_t mutex;
    pthread_t threads[THREADS_MAX];
    int tasks[TASK_QUEUE_SIZE];
    int head;
    int tail;
    int thread_count;
    sem_t task_count;
    sem_t task_space;
    atomic_int pending_tasks;
    pthread_mutex_t wait_mutex;
    pthread_cond_t wait_cond;
} ThreadPool;

typedef struct {
    void (*task_function)(int task);
    ThreadPool* pool;
} ThreadArgs;

void *thread_pool_proc(void *args_void);
int thread_pool_create(ThreadPool* pool, int thread_count, void(*task_function)(int task));
void thread_pool_add_task(ThreadPool *pool, int task);
void thread_pool_wait_all_tasks(ThreadPool *pool);

#endif
