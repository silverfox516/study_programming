// Monitor Object Pattern in C
// Synchronized bounded buffer using mutex + condition variables

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define CAPACITY 4

typedef struct {
    int items[CAPACITY];
    int count, in_idx, out_idx;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Monitor;

void monitor_init(Monitor* m) {
    m->count = m->in_idx = m->out_idx = 0;
    pthread_mutex_init(&m->mutex, NULL);
    pthread_cond_init(&m->not_empty, NULL);
    pthread_cond_init(&m->not_full, NULL);
}

void monitor_put(Monitor* m, int item) {
    pthread_mutex_lock(&m->mutex);
    while (m->count == CAPACITY)
        pthread_cond_wait(&m->not_full, &m->mutex);

    m->items[m->in_idx] = item;
    m->in_idx = (m->in_idx + 1) % CAPACITY;
    m->count++;
    printf("[Monitor] Put %d (count=%d)\n", item, m->count);
    pthread_cond_signal(&m->not_empty);
    pthread_mutex_unlock(&m->mutex);
}

int monitor_take(Monitor* m) {
    pthread_mutex_lock(&m->mutex);
    while (m->count == 0)
        pthread_cond_wait(&m->not_empty, &m->mutex);

    int item = m->items[m->out_idx];
    m->out_idx = (m->out_idx + 1) % CAPACITY;
    m->count--;
    printf("[Monitor] Take %d (count=%d)\n", item, m->count);
    pthread_cond_signal(&m->not_full);
    pthread_mutex_unlock(&m->mutex);
    return item;
}

void monitor_destroy(Monitor* m) {
    pthread_mutex_destroy(&m->mutex);
    pthread_cond_destroy(&m->not_empty);
    pthread_cond_destroy(&m->not_full);
}

static Monitor mon;

void* producer(void* arg) {
    for (int i = 0; i < 8; i++) {
        monitor_put(&mon, i);
        usleep(20000);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 8; i++) {
        int val = monitor_take(&mon);
        printf("  Consumed: %d\n", val);
        usleep(40000);
    }
    return NULL;
}

int main(void) {
    monitor_init(&mon);
    pthread_t p, c;
    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    monitor_destroy(&mon);
    return 0;
}
