// Half-Sync/Half-Async Pattern in C
// Async layer queues work, sync worker threads process it

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_QUEUE 32

typedef struct {
    int id;
    char payload[64];
} Request;

typedef struct {
    Request queue[MAX_QUEUE];
    int count;
    bool done;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} AsyncQueue;

void queue_init(AsyncQueue* q) {
    q->count = 0;
    q->done = false;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void queue_submit(AsyncQueue* q, int id, const char* payload) {
    pthread_mutex_lock(&q->mutex);
    if (q->count < MAX_QUEUE) {
        q->queue[q->count].id = id;
        snprintf(q->queue[q->count].payload, 64, "%s", payload);
        q->count++;
        printf("[Async] Queued request %d\n", id);
        pthread_cond_signal(&q->cond);
    }
    pthread_mutex_unlock(&q->mutex);
}

bool queue_take(AsyncQueue* q, Request* out) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0 && !q->done)
        pthread_cond_wait(&q->cond, &q->mutex);

    if (q->count == 0 && q->done) {
        pthread_mutex_unlock(&q->mutex);
        return false;
    }

    *out = q->queue[0];
    for (int i = 1; i < q->count; i++)
        q->queue[i - 1] = q->queue[i];
    q->count--;
    pthread_mutex_unlock(&q->mutex);
    return true;
}

void queue_close(AsyncQueue* q) {
    pthread_mutex_lock(&q->mutex);
    q->done = true;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

void queue_destroy(AsyncQueue* q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

void* sync_worker(void* arg) {
    void** args = (void**)arg;
    AsyncQueue* q = (AsyncQueue*)args[0];
    int id = *(int*)args[1];

    Request req;
    while (queue_take(q, &req)) {
        printf("[Sync Worker %d] Processing request %d: %s\n", id, req.id, req.payload);
        usleep(80000);
        printf("[Sync Worker %d] Done with request %d\n", id, req.id);
    }
    return NULL;
}

int main(void) {
    AsyncQueue queue;
    queue_init(&queue);

    pthread_t workers[2];
    int ids[] = {0, 1};
    void* args0[] = {&queue, &ids[0]};
    void* args1[] = {&queue, &ids[1]};

    pthread_create(&workers[0], NULL, sync_worker, args0);
    pthread_create(&workers[1], NULL, sync_worker, args1);

    // Async layer submits requests
    for (int i = 0; i < 6; i++) {
        char payload[32];
        snprintf(payload, sizeof(payload), "task-%d", i);
        queue_submit(&queue, i, payload);
    }

    usleep(200000);
    queue_close(&queue);

    pthread_join(workers[0], NULL);
    pthread_join(workers[1], NULL);

    queue_destroy(&queue);
    printf("All done.\n");
    return 0;
}
