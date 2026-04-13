// Retry Pattern in C

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

typedef bool (*Operation)(int attempt);

bool retry(int max_attempts, int delay_us, Operation op) {
    for (int i = 1; i <= max_attempts; i++) {
        if (op(i)) return true;
        printf("[Retry] Attempt %d/%d failed\n", i, max_attempts);
        if (i < max_attempts) usleep(delay_us);
    }
    return false;
}

static int call_count = 0;

bool flaky_service(int attempt) {
    call_count++;
    if (call_count < 3) { printf("  Connection refused\n"); return false; }
    printf("  Connected!\n");
    return true;
}

int main(void) {
    printf("=== Retry (max 5) ===\n");
    bool ok = retry(5, 50000, flaky_service);
    printf("Result: %s\n", ok ? "success" : "failed");
    return 0;
}
