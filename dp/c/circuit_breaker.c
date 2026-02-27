#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Service Call Function Pointer Type
typedef bool (*ServiceCall)(void);

typedef enum {
    STATE_CLOSED,     // Variable normal operation (all good)
    STATE_OPEN,       // Circuit open (failing fast)
    STATE_HALF_OPEN   // Testing if service recovers
} CircuitState;

typedef struct {
    CircuitState state;
    int failure_count;
    int failure_threshold;
    time_t last_failure_time;
    int recovery_timeout; // Seconds
} CircuitBreaker;

// --- Circuit Breaker Implementation ---

CircuitBreaker* create_circuit_breaker(int threshold, int timeout_sec) {
    CircuitBreaker* cb = malloc(sizeof(CircuitBreaker));
    cb->state = STATE_CLOSED;
    cb->failure_count = 0;
    cb->failure_threshold = threshold;
    cb->last_failure_time = 0;
    cb->recovery_timeout = timeout_sec;
    return cb;
}

void execute_request(CircuitBreaker* cb, ServiceCall service_func) {
    time_t now = time(NULL);
    
    // Check State
    if (cb->state == STATE_OPEN) {
        if (difftime(now, cb->last_failure_time) > cb->recovery_timeout) {
            printf("[Circuit] Timeout passed, switching to HALF-OPEN.\n");
            cb->state = STATE_HALF_OPEN;
        } else {
            printf("[Circuit] Fast Fail (OPEN). Request blocked.\n");
            return;
        }
    }
    
    // Attempt Call
    bool success = service_func();
    
    if (success) {
        if (cb->state == STATE_HALF_OPEN) {
            printf("[Circuit] Success in HALF-OPEN. Closing circuit.\n");
            cb->state = STATE_CLOSED;
            cb->failure_count = 0;
        } else {
            printf("Service call success.\n");
            // In CLOSED state, we might want to reset failure count on success, 
            // or keep a rolling window. Simple version resets here.
            cb->failure_count = 0; 
        }
    } else {
        printf("Service call FAILED.\n");
        cb->failure_count++;
        cb->last_failure_time = now;
        
        if (cb->state == STATE_HALF_OPEN) {
            printf("[Circuit] Failure in HALF-OPEN. Re-opening circuit.\n");
            cb->state = STATE_OPEN;
        } else if (cb->failure_count >= cb->failure_threshold) {
             printf("[Circuit] Failure limit reached. Opening circuit!\n");
             cb->state = STATE_OPEN;
        }
    }
}

// --- Simulated Service ---

int service_health_score = 100;

bool unreliable_service() {
    // Returns true if "healthy", false if "failed"
    // For demo: fails if health < 50
    return service_health_score >= 50;
}

// --- Usage Example ---

int main() {
    printf("=== Circuit Breaker Pattern (C) ===\n");
    
    // Threshold 3 failures, 2 seconds timeout for demo speed
    CircuitBreaker* cb = create_circuit_breaker(3, 2); 
    
    printf("\n1. Service is healthy (Score 100)\n");
    execute_request(cb, unreliable_service); // Success
    execute_request(cb, unreliable_service); // Success
    
    printf("\n2. Service degrades (Score 40)\n");
    service_health_score = 40;
    
    execute_request(cb, unreliable_service); // Fail 1
    execute_request(cb, unreliable_service); // Fail 2
    execute_request(cb, unreliable_service); // Fail 3 -> Trips breaker
    
    printf("\n3. Requests during OPEN state\n");
    execute_request(cb, unreliable_service); // Fast failed
    execute_request(cb, unreliable_service); // Fast failed
    
    printf("\nWaiting for recovery timeout (3s)...\n");
    #ifdef _WIN32
        _sleep(3000); // ms
    #else
        sleep(3);
    #endif
    
    printf("\n4. Service recovers (Score 90)\n");
    service_health_score = 90;
    
    // First request after timeout should be HALF-OPEN
    execute_request(cb, unreliable_service); // Success -> Resets to CLOSED
    
    execute_request(cb, unreliable_service); // Normal success
    
    free(cb);
    return 0;
}
