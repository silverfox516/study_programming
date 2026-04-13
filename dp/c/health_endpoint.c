// Health Endpoint Pattern in C

#include <stdio.h>
#include <string.h>

typedef enum { HEALTHY, DEGRADED, UNHEALTHY } HealthStatus;

typedef struct {
    const char* name;
    HealthStatus status;
    const char* message;
    int latency_ms;
} ComponentHealth;

typedef ComponentHealth (*HealthCheck)(void);

#define MAX_CHECKS 8

typedef struct {
    HealthCheck checks[MAX_CHECKS];
    int count;
} HealthChecker;

void checker_init(HealthChecker* hc) { hc->count = 0; }
void checker_add(HealthChecker* hc, HealthCheck check) { hc->checks[hc->count++] = check; }

ComponentHealth check_database(void) {
    return (ComponentHealth){"database", HEALTHY, "Pool: 5/20", 12};
}
ComponentHealth check_cache(void) {
    return (ComponentHealth){"cache", HEALTHY, "Hit rate: 94%", 2};
}
ComponentHealth check_api(void) {
    return (ComponentHealth){"payment_api", DEGRADED, "Elevated latency", 850};
}

const char* status_str(HealthStatus s) {
    switch (s) { case HEALTHY: return "HEALTHY"; case DEGRADED: return "DEGRADED"; case UNHEALTHY: return "UNHEALTHY"; }
    return "UNKNOWN";
}

int main(void) {
    HealthChecker hc;
    checker_init(&hc);
    checker_add(&hc, check_database);
    checker_add(&hc, check_cache);
    checker_add(&hc, check_api);

    HealthStatus overall = HEALTHY;
    printf("=== Health Check ===\n");
    for (int i = 0; i < hc.count; i++) {
        ComponentHealth ch = hc.checks[i]();
        printf("  %s: %s (%dms) - %s\n", ch.name, status_str(ch.status), ch.latency_ms, ch.message);
        if (ch.status == UNHEALTHY) overall = UNHEALTHY;
        else if (ch.status == DEGRADED && overall != UNHEALTHY) overall = DEGRADED;
    }
    printf("Overall: %s\n", status_str(overall));
    return 0;
}
