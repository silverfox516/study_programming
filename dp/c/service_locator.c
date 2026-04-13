// Service Locator Pattern in C

#include <stdio.h>
#include <string.h>

#define MAX_SERVICES 16

typedef struct {
    const char* name;
    void* instance;
} ServiceEntry;

typedef struct {
    ServiceEntry entries[MAX_SERVICES];
    int count;
} ServiceLocator;

void sl_init(ServiceLocator* sl) { sl->count = 0; }

void sl_register(ServiceLocator* sl, const char* name, void* instance) {
    sl->entries[sl->count++] = (ServiceEntry){name, instance};
}

void* sl_get(ServiceLocator* sl, const char* name) {
    for (int i = 0; i < sl->count; i++)
        if (strcmp(sl->entries[i].name, name) == 0)
            return sl->entries[i].instance;
    return NULL;
}

// Example services
typedef struct { const char* prefix; } Logger;
void logger_log(Logger* l, const char* msg) {
    printf("[%s] %s\n", l->prefix, msg);
}

typedef struct { const char* url; } Database;
void db_query(Database* db, const char* sql) {
    printf("[DB:%s] %s\n", db->url, sql);
}

int main(void) {
    ServiceLocator sl;
    sl_init(&sl);

    Logger logger = {"APP"};
    Database db = {"postgres://localhost"};

    sl_register(&sl, "logger", &logger);
    sl_register(&sl, "database", &db);

    Logger* l = (Logger*)sl_get(&sl, "logger");
    if (l) logger_log(l, "Application started");

    Database* d = (Database*)sl_get(&sl, "database");
    if (d) db_query(d, "SELECT * FROM users");

    void* missing = sl_get(&sl, "cache");
    printf("cache found: %s\n", missing ? "yes" : "no");
    return 0;
}
