// Throttling Pattern in C

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int max_requests;
    double window_sec;
    int request_count;
    struct timespec window_start;
} RateLimiter;

void rl_init(RateLimiter* rl, int max_req, double window_sec) {
    rl->max_requests = max_req;
    rl->window_sec = window_sec;
    rl->request_count = 0;
    clock_gettime(CLOCK_MONOTONIC, &rl->window_start);
}

static double elapsed_sec(struct timespec* start) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - start->tv_sec) + (now.tv_nsec - start->tv_nsec) / 1e9;
}

bool rl_try_acquire(RateLimiter* rl) {
    if (elapsed_sec(&rl->window_start) >= rl->window_sec) {
        rl->request_count = 0;
        clock_gettime(CLOCK_MONOTONIC, &rl->window_start);
    }
    if (rl->request_count < rl->max_requests) {
        rl->request_count++;
        return true;
    }
    return false;
}

int main(void) {
    RateLimiter rl;
    rl_init(&rl, 3, 0.2);

    printf("=== Rate Limiting (3 req / 200ms) ===\n");
    for (int i = 0; i < 5; i++) {
        if (rl_try_acquire(&rl))
            printf("[Request %d] OK\n", i);
        else
            printf("[Request %d] REJECTED\n", i);
    }

    printf("\n[Waiting 250ms...]\n");
    usleep(250000);

    for (int i = 5; i < 8; i++) {
        if (rl_try_acquire(&rl))
            printf("[Request %d] OK\n", i);
        else
            printf("[Request %d] REJECTED\n", i);
    }
    return 0;
}
