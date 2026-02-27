#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define POOL_SIZE 5

// The "Resource" we are pooling (e.g., Database Connection)
typedef struct {
    int id;
    bool in_use;
    char data[32];
} Resource;

typedef struct {
    Resource resources[POOL_SIZE];
} ObjectPool;

// --- Object Pool Implementation ---

ObjectPool* create_pool() {
    ObjectPool* pool = malloc(sizeof(ObjectPool));
    for (int i = 0; i < POOL_SIZE; i++) {
        pool->resources[i].id = i + 1;
        pool->resources[i].in_use = false;
        sprintf(pool->resources[i].data, "Connection-%d", i + 1);
    }
    printf("[Pool] Created pool with %d resources\n", POOL_SIZE);
    return pool;
}

Resource* pool_acquire(ObjectPool* pool) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!pool->resources[i].in_use) {
            pool->resources[i].in_use = true;
            printf("[Pool] Acquired Resource ID %d\n", pool->resources[i].id);
            return &pool->resources[i];
        }
    }
    printf("[Pool] All resources passed out! Failed to acquire.\n");
    return NULL;
}

void pool_release(ObjectPool* pool, Resource* resource) {
    if (!resource) return;
    
    // In a real generic pool, we'd verify the resource belongs to the pool.
    // For this simple example, we just mark it as unused.
    resource->in_use = false;
    printf("[Pool] Released Resource ID %d\n", resource->id);
}

void destroy_pool(ObjectPool* pool) {
    free(pool);
    printf("[Pool] Destroyed.\n");
}

// --- Usage Example ---

int main() {
    printf("=== Object Pool Pattern (C) ===\n");
    
    ObjectPool* db_pool = create_pool();
    
    // Simulate high traffic
    printf("\n--- Phase 1: High Demand ---\n");
    Resource* r1 = pool_acquire(db_pool);
    Resource* r2 = pool_acquire(db_pool);
    Resource* r3 = pool_acquire(db_pool);
    
    if (r1) printf("Using %s...\n", r1->data);
    
    Resource* r4 = pool_acquire(db_pool);
    Resource* r5 = pool_acquire(db_pool);
    Resource* r6 = pool_acquire(db_pool); // Should fail
    
    // Simulate cleanup
    printf("\n--- Phase 2: Releasing Resources ---\n");
    pool_release(db_pool, r2);
    pool_release(db_pool, r4);
    
    // Reuse
    printf("\n--- Phase 3: Reuse ---\n");
    Resource* r7 = pool_acquire(db_pool); // Should get one of the released ones
    if (r7) printf("Reusing Resource ID %d\n", r7->id);
    
    destroy_pool(db_pool);
    return 0;
}
