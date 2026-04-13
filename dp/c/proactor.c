// Proactor Pattern in C
// Asynchronous operations with completion handlers (simulated via thread)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char operation[64];
    char result[128];
    int success;
} AsyncResult;

typedef void (*CompletionHandler)(const AsyncResult*);

typedef struct {
    char operation[64];
    CompletionHandler handler;
} AsyncRequest;

void* async_worker(void* arg) {
    AsyncRequest* req = (AsyncRequest*)arg;

    // Simulate async I/O
    printf("[Proactor] Starting: %s\n", req->operation);
    usleep(100000);

    AsyncResult result;
    snprintf(result.operation, sizeof(result.operation), "%s", req->operation);
    snprintf(result.result, sizeof(result.result), "Result of '%s'", req->operation);
    result.success = 1;

    req->handler(&result);
    free(req);
    return NULL;
}

void proactor_submit(const char* operation, CompletionHandler handler) {
    AsyncRequest* req = malloc(sizeof(AsyncRequest));
    snprintf(req->operation, sizeof(req->operation), "%s", operation);
    req->handler = handler;

    pthread_t t;
    pthread_create(&t, NULL, async_worker, req);
    pthread_detach(t);
}

void on_read_complete(const AsyncResult* r) {
    printf("  [Complete] %s: %s\n", r->operation, r->result);
}

void on_write_complete(const AsyncResult* r) {
    printf("  [Complete] %s: success=%d\n", r->operation, r->success);
}

int main(void) {
    proactor_submit("read(/etc/config)", on_read_complete);
    proactor_submit("write(/tmp/out)", on_write_complete);
    proactor_submit("read(/var/log/app)", on_read_complete);

    usleep(500000);
    printf("All async operations completed.\n");
    return 0;
}
