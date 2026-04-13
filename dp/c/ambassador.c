// Ambassador Pattern in C

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef bool (*ServiceCall)(const char* request, char* response, int resp_size);

bool real_service(const char* request, char* response, int resp_size) {
    if (strstr(request, "fail")) return false;
    snprintf(response, resp_size, "Response for '%s'", request);
    return true;
}

bool ambassador_call(ServiceCall svc, const char* request, char* response, int resp_size, int max_retries) {
    printf("[Ambassador] -> %s\n", request);

    for (int attempt = 1; attempt <= max_retries; attempt++) {
        if (svc(request, response, resp_size)) {
            printf("[Ambassador] <- OK (attempt %d): %s\n", attempt, response);
            return true;
        }
        printf("[Ambassador] Attempt %d failed\n", attempt);
    }

    printf("[Ambassador] All retries exhausted\n");
    return false;
}

int main(void) {
    char response[128];

    printf("=== Successful ===\n");
    ambassador_call(real_service, "get_users", response, sizeof(response), 3);

    printf("\n=== Failing ===\n");
    ambassador_call(real_service, "fail_please", response, sizeof(response), 3);
    return 0;
}
