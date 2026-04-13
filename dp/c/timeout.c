// Timeout Pattern in C

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

typedef struct { int result; bool completed; } TaskResult;

typedef struct {
    int work_ms;
    TaskResult* result;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
} TaskArg;

void* timed_task(void* arg) {
    TaskArg* ta = (TaskArg*)arg;
    usleep(ta->work_ms * 1000);

    pthread_mutex_lock(ta->mutex);
    ta->result->result = 42;
    ta->result->completed = true;
    pthread_cond_signal(ta->cond);
    pthread_mutex_unlock(ta->mutex);
    return NULL;
}

bool run_with_timeout(int work_ms, int timeout_ms, int* out) {
    TaskResult result = {0, false};
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    TaskArg arg = {work_ms, &result, &mutex, &cond};
    pthread_t thread;
    pthread_create(&thread, NULL, timed_task, &arg);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += (long)timeout_ms * 1000000L;
    ts.tv_sec += ts.tv_nsec / 1000000000L;
    ts.tv_nsec %= 1000000000L;

    pthread_mutex_lock(&mutex);
    while (!result.completed) {
        int rc = pthread_cond_timedwait(&cond, &mutex, &ts);
        if (rc != 0) break;
    }
    bool ok = result.completed;
    pthread_mutex_unlock(&mutex);

    if (ok) *out = result.result;
    pthread_detach(thread);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return ok;
}

int main(void) {
    int result;

    printf("=== Fast (50ms work, 200ms timeout) ===\n");
    if (run_with_timeout(50, 200, &result))
        printf("Success: %d\n", result);
    else
        printf("Timeout!\n");

    printf("\n=== Slow (500ms work, 100ms timeout) ===\n");
    if (run_with_timeout(500, 100, &result))
        printf("Success: %d\n", result);
    else
        printf("Timeout!\n");

    usleep(600000); // Let detached threads finish
    return 0;
}
