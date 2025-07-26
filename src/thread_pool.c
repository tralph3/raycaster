#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

#include "thread_pool.h"

void *thread_pool_proc(void *args_void) {
    ThreadArgs *args = (ThreadArgs*)args_void;
    ThreadPool *pool = args->pool;
    void (*task_function)(int) = args->task_function;
    free(args);

    while (true) {
        sem_wait(&pool->task_count);

        pthread_mutex_lock(&pool->mutex);
        int task = pool->tasks[pool->head];
        pool->head = (pool->head + 1) % TASK_QUEUE_SIZE;
        pthread_mutex_unlock(&pool->mutex);

        sem_post(&pool->task_space);
        task_function(task);

        if (atomic_fetch_sub(&pool->pending_tasks, 1) == 1) {
            pthread_mutex_lock(&pool->wait_mutex);
            pthread_cond_signal(&pool->wait_cond);
            pthread_mutex_unlock(&pool->wait_mutex);
        }
    }

    return NULL;
}

int thread_pool_create(ThreadPool* pool, int thread_count, void(*task_function)(int task)) {
    if (thread_count >= THREADS_MAX) {
        return 0;
    }

    pool->head = 0;
    pool->tail = 0;
    atomic_init(&pool->pending_tasks, 0);
    pthread_mutex_init(&pool->wait_mutex, NULL);
    pthread_cond_init(&pool->wait_cond, NULL);

    pool->thread_count = thread_count;
    sem_init(&pool->task_count, 0, 0);
    sem_init(&pool->task_space, 0, TASK_QUEUE_SIZE);
    pthread_mutex_init(&pool->mutex, NULL);

    for (int i = 0; i < pool->thread_count; ++i) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->task_function = task_function;
        args->pool = pool;

        pthread_create(&pool->threads[i], NULL, thread_pool_proc, (void*)args);
        pthread_detach(pool->threads[i]);
    }

    return 1;
}

void thread_pool_add_task(ThreadPool *pool, int task) {
    sem_wait(&pool->task_space);
    pthread_mutex_lock(&pool->mutex);

    pool->tasks[pool->tail] = task;
    pool->tail = (pool->tail + 1) % TASK_QUEUE_SIZE;

    atomic_fetch_add(&pool->pending_tasks, 1);

    pthread_mutex_unlock(&pool->mutex);
    sem_post(&pool->task_count);
}

void thread_pool_wait_all_tasks(ThreadPool *pool) {
    pthread_mutex_lock(&pool->wait_mutex);
    while (atomic_load(&pool->pending_tasks) > 0) {
        pthread_cond_wait(&pool->wait_cond, &pool->wait_mutex);
    }
    pthread_mutex_unlock(&pool->wait_mutex);
}
