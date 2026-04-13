// Policy-Based Design in C
// Compose behavior via interchangeable function pointer tables

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Log policy
typedef struct { void (*log)(const char* msg); } LogPolicy;

void console_log(const char* msg) { printf("[Console] %s\n", msg); }
void silent_log(const char* msg) { (void)msg; }

static const LogPolicy CONSOLE_LOG = {console_log};
static const LogPolicy SILENT_LOG = {silent_log};

// Validation policy
typedef struct { bool (*validate)(const char* value, char* err, int err_size); } ValidationPolicy;

bool strict_validate(const char* value, char* err, int err_size) {
    if (!value || value[0] == '\0') { snprintf(err, err_size, "empty"); return false; }
    if (strlen(value) > 100) { snprintf(err, err_size, "too long"); return false; }
    if (strchr(value, '<')) { snprintf(err, err_size, "HTML not allowed"); return false; }
    return true;
}

bool lenient_validate(const char* value, char* err, int err_size) {
    if (!value || value[0] == '\0') { snprintf(err, err_size, "empty"); return false; }
    return true;
}

static const ValidationPolicy STRICT = {strict_validate};
static const ValidationPolicy LENIENT = {lenient_validate};

// Composed service
typedef struct {
    const LogPolicy* logger;
    const ValidationPolicy* validator;
} ConfigService;

bool config_set(ConfigService* svc, const char* key, const char* value) {
    char msg[128], err[64];
    snprintf(msg, sizeof(msg), "Setting %s = %s", key, value);
    svc->logger->log(msg);

    if (!svc->validator->validate(value, err, sizeof(err))) {
        snprintf(msg, sizeof(msg), "Validation failed: %s", err);
        svc->logger->log(msg);
        return false;
    }

    snprintf(msg, sizeof(msg), "Saved %s", key);
    svc->logger->log(msg);
    return true;
}

int main(void) {
    printf("=== Strict + Console ===\n");
    ConfigService strict_svc = {&CONSOLE_LOG, &STRICT};
    config_set(&strict_svc, "name", "Alice");
    config_set(&strict_svc, "bad", "<script>");

    printf("\n=== Lenient + Silent ===\n");
    ConfigService lenient_svc = {&SILENT_LOG, &LENIENT};
    bool ok = config_set(&lenient_svc, "html", "<b>bold</b>");
    printf("Result: %s\n", ok ? "saved" : "failed");
    return 0;
}
