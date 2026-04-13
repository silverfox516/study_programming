// Bulkhead Pattern in C

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct {
    const char* name;
    int max;
    int active;
    pthread_mutex_t mutex;
} Bulkhead;

void bh_init(Bulkhead* b, const char* name, int max) {
    b->name = name;
    b->max = max;
    b->active = 0;
    pthread_mutex_init(&b->mutex, NULL);
}

bool bh_acquire(Bulkhead* b) {
    pthread_mutex_lock(&b->mutex);
    if (b->active >= b->max) {
        printf("[%s] REJECTED (%d/%d)\n", b->name, b->active, b->max);
        pthread_mutex_unlock(&b->mutex);
        return false;
    }
    b->active++;
    printf("[%s] Acquired (%d/%d)\n", b->name, b->active, b->max);
    pthread_mutex_unlock(&b->mutex);
    return true;
}

void bh_release(Bulkhead* b) {
    pthread_mutex_lock(&b->mutex);
    b->active--;
    printf("[%s] Released (%d/%d)\n", b->name, b->active, b->max);
    pthread_mutex_unlock(&b->mutex);
}

void bh_destroy(Bulkhead* b) { pthread_mutex_destroy(&b->mutex); }

static Bulkhead api_bh;

void* worker(void* arg) {
    int id = *(int*)arg;
    if (bh_acquire(&api_bh)) {
        printf("  [Task %d] Processing...\n", id);
        usleep(100000);
        bh_release(&api_bh);
    }
    return NULL;
}

int main(void) {
    bh_init(&api_bh, "API", 2);

    pthread_t threads[5];
    int ids[] = {0, 1, 2, 3, 4};
    for (int i = 0; i < 5; i++)
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    for (int i = 0; i < 5; i++)
        pthread_join(threads[i], NULL);

    bh_destroy(&api_bh);
    return 0;
}
