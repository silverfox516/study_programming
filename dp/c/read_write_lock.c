// Read-Write Lock Pattern in C
// Multiple concurrent readers OR exclusive writer using pthread_rwlock

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_ENTRIES 32

typedef struct {
    char key[32];
    char value[64];
} Entry;

typedef struct {
    Entry entries[MAX_ENTRIES];
    int count;
    pthread_rwlock_t lock;
} SharedCache;

void cache_init(SharedCache* c) {
    c->count = 0;
    pthread_rwlock_init(&c->lock, NULL);
}

void cache_write(SharedCache* c, const char* key, const char* value) {
    pthread_rwlock_wrlock(&c->lock);
    for (int i = 0; i < c->count; i++) {
        if (strcmp(c->entries[i].key, key) == 0) {
            snprintf(c->entries[i].value, 64, "%s", value);
            printf("[Writer] Updated: %s=%s\n", key, value);
            pthread_rwlock_unlock(&c->lock);
            return;
        }
    }
    if (c->count < MAX_ENTRIES) {
        snprintf(c->entries[c->count].key, 32, "%s", key);
        snprintf(c->entries[c->count].value, 64, "%s", value);
        c->count++;
        printf("[Writer] Added: %s=%s\n", key, value);
    }
    pthread_rwlock_unlock(&c->lock);
}

const char* cache_read(SharedCache* c, const char* key) {
    pthread_rwlock_rdlock(&c->lock);
    for (int i = 0; i < c->count; i++) {
        if (strcmp(c->entries[i].key, key) == 0) {
            const char* val = c->entries[i].value;
            pthread_rwlock_unlock(&c->lock);
            return val;
        }
    }
    pthread_rwlock_unlock(&c->lock);
    return NULL;
}

void cache_destroy(SharedCache* c) {
    pthread_rwlock_destroy(&c->lock);
}

static SharedCache cache;

void* writer_fn(void* arg) {
    for (int i = 0; i < 5; i++) {
        char key[16], val[32];
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(val, sizeof(val), "value%d", i);
        cache_write(&cache, key, val);
        usleep(50000);
    }
    return NULL;
}

void* reader_fn(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        const char* val = cache_read(&cache, key);
        printf("[Reader %d] %s = %s\n", id, key, val ? val : "(null)");
        usleep(30000);
    }
    return NULL;
}

int main(void) {
    cache_init(&cache);

    pthread_t w, r1, r2;
    int id1 = 1, id2 = 2;
    pthread_create(&w, NULL, writer_fn, NULL);
    pthread_create(&r1, NULL, reader_fn, &id1);
    pthread_create(&r2, NULL, reader_fn, &id2);

    pthread_join(w, NULL);
    pthread_join(r1, NULL);
    pthread_join(r2, NULL);

    cache_destroy(&cache);
    printf("Cache size: %d\n", cache.count);
    return 0;
}
