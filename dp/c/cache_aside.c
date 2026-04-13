// Cache-Aside Pattern in C

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CACHE 16

typedef struct { char key[32]; char value[64]; bool valid; } CacheEntry;

typedef struct { CacheEntry entries[MAX_CACHE]; int count; } Cache;

void cache_init(Cache* c) { c->count = 0; }

const char* cache_get(Cache* c, const char* key) {
    for (int i = 0; i < c->count; i++)
        if (c->entries[i].valid && strcmp(c->entries[i].key, key) == 0) {
            printf("[Cache] HIT: %s\n", key);
            return c->entries[i].value;
        }
    printf("[Cache] MISS: %s\n", key);
    return NULL;
}

void cache_put(Cache* c, const char* key, const char* value) {
    for (int i = 0; i < c->count; i++)
        if (strcmp(c->entries[i].key, key) == 0) {
            snprintf(c->entries[i].value, 64, "%s", value);
            c->entries[i].valid = true;
            return;
        }
    snprintf(c->entries[c->count].key, 32, "%s", key);
    snprintf(c->entries[c->count].value, 64, "%s", value);
    c->entries[c->count].valid = true;
    c->count++;
    printf("[Cache] PUT: %s=%s\n", key, value);
}

void cache_invalidate(Cache* c, const char* key) {
    for (int i = 0; i < c->count; i++)
        if (strcmp(c->entries[i].key, key) == 0)
            c->entries[i].valid = false;
    printf("[Cache] INVALIDATE: %s\n", key);
}

// Simulated DB
const char* db_query(const char* key) {
    printf("[DB] Query: %s\n", key);
    if (strcmp(key, "user:1") == 0) return "Alice";
    if (strcmp(key, "user:2") == 0) return "Bob";
    return NULL;
}

const char* get_user(Cache* c, const char* key) {
    const char* val = cache_get(c, key);
    if (val) return val;
    val = db_query(key);
    if (val) cache_put(c, key, val);
    return val;
}

int main(void) {
    Cache cache;
    cache_init(&cache);

    printf("=== First access ===\n");
    printf("Result: %s\n\n", get_user(&cache, "user:1"));

    printf("=== Second access (cached) ===\n");
    printf("Result: %s\n\n", get_user(&cache, "user:1"));

    printf("=== After invalidation ===\n");
    cache_invalidate(&cache, "user:1");
    printf("Result: %s\n", get_user(&cache, "user:1"));
    return 0;
}
