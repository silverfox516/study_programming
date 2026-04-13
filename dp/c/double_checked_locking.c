// Double-Checked Locking Pattern in C
// Thread-safe lazy initialization with minimal locking overhead

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    char db_url[128];
    int max_connections;
} Config;

static atomic_intptr_t g_config = ATOMIC_VAR_INIT(0);
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

Config* get_config(void) {
    Config* cfg = (Config*)atomic_load(&g_config);

    // First check (no lock)
    if (!cfg) {
        pthread_mutex_lock(&g_mutex);

        // Second check (with lock)
        cfg = (Config*)atomic_load(&g_config);
        if (!cfg) {
            printf("[Config] Initializing (expensive)...\n");
            cfg = malloc(sizeof(Config));
            snprintf(cfg->db_url, sizeof(cfg->db_url), "postgres://localhost/mydb");
            cfg->max_connections = 10;
            atomic_store(&g_config, (intptr_t)cfg);
        }

        pthread_mutex_unlock(&g_mutex);
    }

    return cfg;
}

// pthread_once alternative (recommended in practice)
static pthread_once_t once_ctrl = PTHREAD_ONCE_INIT;
static Config* once_config = NULL;

void init_once_config(void) {
    printf("[Once] Initializing config...\n");
    once_config = malloc(sizeof(Config));
    snprintf(once_config->db_url, sizeof(once_config->db_url), "postgres://localhost/db2");
    once_config->max_connections = 20;
}

Config* get_config_once(void) {
    pthread_once(&once_ctrl, init_once_config);
    return once_config;
}

int main(void) {
    printf("=== Double-Checked Locking ===\n");
    Config* c1 = get_config();
    Config* c2 = get_config(); // Not initialized again
    printf("URL: %s\n", c1->db_url);
    printf("Same instance: %s\n", c1 == c2 ? "yes" : "no");

    printf("\n=== pthread_once ===\n");
    Config* c3 = get_config_once();
    Config* c4 = get_config_once();
    printf("URL: %s\n", c3->db_url);
    printf("Same instance: %s\n", c3 == c4 ? "yes" : "no");

    free(c1);
    free(once_config);
    return 0;
}
