// Producer-Consumer Pattern in C
// Thread-safe bounded buffer using pthread mutex + condition variables

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} BoundedBuffer;

void buffer_init(BoundedBuffer* b) {
    b->count = 0;
    b->in = 0;
    b->out = 0;
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->not_empty, NULL);
    pthread_cond_init(&b->not_full, NULL);
}

void buffer_put(BoundedBuffer* b, int item) {
    pthread_mutex_lock(&b->mutex);
    while (b->count == BUFFER_SIZE)
        pthread_cond_wait(&b->not_full, &b->mutex);

    b->buffer[b->in] = item;
    b->in = (b->in + 1) % BUFFER_SIZE;
    b->count++;
    printf("[Producer] Put: %d (count=%d)\n", item, b->count);

    pthread_cond_signal(&b->not_empty);
    pthread_mutex_unlock(&b->mutex);
}

int buffer_get(BoundedBuffer* b) {
    pthread_mutex_lock(&b->mutex);
    while (b->count == 0)
        pthread_cond_wait(&b->not_empty, &b->mutex);

    int item = b->buffer[b->out];
    b->out = (b->out + 1) % BUFFER_SIZE;
    b->count--;
    printf("[Consumer] Got: %d (count=%d)\n", item, b->count);

    pthread_cond_signal(&b->not_full);
    pthread_mutex_unlock(&b->mutex);
    return item;
}

void buffer_destroy(BoundedBuffer* b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->not_empty);
    pthread_cond_destroy(&b->not_full);
}

static BoundedBuffer shared_buffer;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = id * 100 + i;
        buffer_put(&shared_buffer, item);
        usleep(20000);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < NUM_ITEMS * 2; i++) {
        buffer_get(&shared_buffer);
        usleep(30000);
    }
    return NULL;
}

int main(void) {
    buffer_init(&shared_buffer);

    pthread_t prod1, prod2, cons;
    int id1 = 1, id2 = 2;

    pthread_create(&prod1, NULL, producer, &id1);
    pthread_create(&prod2, NULL, producer, &id2);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons, NULL);

    buffer_destroy(&shared_buffer);
    printf("Done.\n");
    return 0;
}
