// Leader-Followers Pattern in C
// Workers compete for leadership; leader waits for events, processes, then yields

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_EVENTS 32
#define NUM_WORKERS 3

typedef struct {
    char events[MAX_EVENTS][64];
    int event_count;
    int current_leader;  // -1 = none
    bool shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} LeaderFollowers;

void lf_init(LeaderFollowers* lf) {
    lf->event_count = 0;
    lf->current_leader = -1;
    lf->shutdown = false;
    pthread_mutex_init(&lf->mutex, NULL);
    pthread_cond_init(&lf->cond, NULL);
}

void* lf_worker(void* arg) {
    LeaderFollowers* lf = ((void**)arg)[0];
    int id = *(int*)((void**)arg)[1];
    free(((void**)arg)[1]);
    free(arg);

    while (1) {
        pthread_mutex_lock(&lf->mutex);

        while (lf->current_leader >= 0 && !lf->shutdown)
            pthread_cond_wait(&lf->cond, &lf->mutex);

        if (lf->shutdown) { pthread_mutex_unlock(&lf->mutex); break; }

        lf->current_leader = id;
        printf("[Worker %d] Became LEADER\n", id);

        while (lf->event_count == 0 && !lf->shutdown)
            pthread_cond_wait(&lf->cond, &lf->mutex);

        if (lf->shutdown) { pthread_mutex_unlock(&lf->mutex); break; }

        char event[64];
        snprintf(event, sizeof(event), "%s", lf->events[0]);
        for (int i = 1; i < lf->event_count; i++)
            snprintf(lf->events[i - 1], 64, "%s", lf->events[i]);
        lf->event_count--;

        lf->current_leader = -1;
        pthread_cond_broadcast(&lf->cond);
        pthread_mutex_unlock(&lf->mutex);

        printf("[Worker %d] Processing: %s\n", id, event);
        usleep(50000);
    }
    return NULL;
}

void lf_submit(LeaderFollowers* lf, const char* event) {
    pthread_mutex_lock(&lf->mutex);
    if (lf->event_count < MAX_EVENTS) {
        snprintf(lf->events[lf->event_count], 64, "%s", event);
        lf->event_count++;
        pthread_cond_broadcast(&lf->cond);
    }
    pthread_mutex_unlock(&lf->mutex);
}

void lf_shutdown(LeaderFollowers* lf) {
    pthread_mutex_lock(&lf->mutex);
    lf->shutdown = true;
    pthread_cond_broadcast(&lf->cond);
    pthread_mutex_unlock(&lf->mutex);
}

void lf_destroy(LeaderFollowers* lf) {
    pthread_mutex_destroy(&lf->mutex);
    pthread_cond_destroy(&lf->cond);
}

int main(void) {
    LeaderFollowers lf;
    lf_init(&lf);

    pthread_t threads[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; i++) {
        void** args = malloc(2 * sizeof(void*));
        int* id = malloc(sizeof(int));
        *id = i;
        args[0] = &lf;
        args[1] = id;
        pthread_create(&threads[i], NULL, lf_worker, args);
    }

    usleep(50000);
    for (int i = 0; i < 6; i++) {
        char event[64];
        snprintf(event, sizeof(event), "Event-%d", i);
        lf_submit(&lf, event);
        usleep(30000);
    }

    usleep(300000);
    lf_shutdown(&lf);
    for (int i = 0; i < NUM_WORKERS; i++)
        pthread_join(threads[i], NULL);

    lf_destroy(&lf);
    printf("Done.\n");
    return 0;
}
