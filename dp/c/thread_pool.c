// Thread Pool Pattern in C
// Fixed pool of worker threads processing jobs from a queue

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

typedef void (*TaskFunc)(void* arg);

typedef struct Task {
    TaskFunc func;
    void* arg;
    struct Task* next;
} Task;

typedef struct {
    pthread_t* threads;
    int thread_count;
    Task* queue_head;
    Task* queue_tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool shutdown;
} ThreadPool;

void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    while (1) {
        pthread_mutex_lock(&pool->mutex);
        while (!pool->queue_head && !pool->shutdown)
            pthread_cond_wait(&pool->cond, &pool->mutex);

        if (pool->shutdown && !pool->queue_head) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }

        Task* task = pool->queue_head;
        pool->queue_head = task->next;
        if (!pool->queue_head) pool->queue_tail = NULL;
        pthread_mutex_unlock(&pool->mutex);

        task->func(task->arg);
        free(task);
    }
    return NULL;
}

ThreadPool* pool_create(int num_threads) {
    ThreadPool* pool = calloc(1, sizeof(ThreadPool));
    pool->thread_count = num_threads;
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (int i = 0; i < num_threads; i++)
        pthread_create(&pool->threads[i], NULL, worker, pool);
    return pool;
}

void pool_submit(ThreadPool* pool, TaskFunc func, void* arg) {
    Task* task = malloc(sizeof(Task));
    task->func = func;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->mutex);
    if (pool->queue_tail)
        pool->queue_tail->next = task;
    else
        pool->queue_head = task;
    pool->queue_tail = task;
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);
}

void pool_destroy(ThreadPool* pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < pool->thread_count; i++)
        pthread_join(pool->threads[i], NULL);

    free(pool->threads);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    free(pool);
}

void print_task(void* arg) {
    int id = *(int*)arg;
    printf("[Worker %lu] Task %d\n", (unsigned long)pthread_self(), id);
    usleep(50000);
    free(arg);
}

int main(void) {
    ThreadPool* pool = pool_create(3);

    for (int i = 0; i < 10; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pool_submit(pool, print_task, id);
    }

    usleep(500000);
    pool_destroy(pool);
    printf("Pool shutdown.\n");
    return 0;
}
