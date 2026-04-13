// Lazy Initialization Pattern in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int* data;
    int size;
    bool initialized;
} LazyResource;

void lazy_init(LazyResource* r) {
    r->data = NULL;
    r->size = 0;
    r->initialized = false;
}

int* lazy_get(LazyResource* r) {
    if (!r->initialized) {
        printf("[Lazy] Loading data (expensive)...\n");
        r->size = 5;
        r->data = malloc(sizeof(int) * r->size);
        for (int i = 0; i < r->size; i++) r->data[i] = (i + 1) * 10;
        r->initialized = true;
    }
    return r->data;
}

void lazy_free(LazyResource* r) {
    free(r->data);
    r->data = NULL;
    r->initialized = false;
}

// Singleton-style lazy
static char* global_config = NULL;

const char* get_config(void) {
    if (!global_config) {
        printf("[Config] Loading...\n");
        global_config = malloc(64);
        snprintf(global_config, 64, "production");
    }
    return global_config;
}

int main(void) {
    LazyResource res;
    lazy_init(&res);

    printf("=== LazyResource ===\n");
    printf("Before access...\n");
    int* data = lazy_get(&res);
    printf("Data: %d %d %d\n", data[0], data[1], data[2]);
    lazy_get(&res); // Not loaded again
    lazy_free(&res);

    printf("\n=== Global lazy ===\n");
    printf("Config: %s\n", get_config());
    printf("Config: %s\n", get_config()); // Not loaded again
    free(global_config);
    return 0;
}
