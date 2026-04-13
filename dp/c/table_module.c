// Table Module Pattern in C

#include <stdio.h>
#include <string.h>

#define MAX_ROWS 32
#define MAX_FIELDS 8

typedef struct {
    char fields[MAX_FIELDS][32];
    char values[MAX_FIELDS][64];
    int field_count;
} Row;

typedef struct {
    const char* name;
    Row rows[MAX_ROWS];
    int count;
    int next_id;
} TableModule;

void table_init(TableModule* t, const char* name) {
    t->name = name;
    t->count = 0;
    t->next_id = 1;
}

int table_insert(TableModule* t, const char* fields[], const char* values[], int n) {
    int id = t->next_id++;
    Row* r = &t->rows[t->count];
    snprintf(r->fields[0], 32, "id");
    snprintf(r->values[0], 64, "%d", id);
    r->field_count = n + 1;
    for (int i = 0; i < n; i++) {
        snprintf(r->fields[i + 1], 32, "%s", fields[i]);
        snprintf(r->values[i + 1], 64, "%s", values[i]);
    }
    t->count++;
    printf("[%s] INSERT id=%d\n", t->name, id);
    return id;
}

const Row* table_find_by_id(TableModule* t, int id) {
    char id_str[16];
    snprintf(id_str, 16, "%d", id);
    for (int i = 0; i < t->count; i++)
        if (strcmp(t->rows[i].values[0], id_str) == 0)
            return &t->rows[i];
    return NULL;
}

int main(void) {
    TableModule users;
    table_init(&users, "users");

    const char* f1[] = {"name", "role"};
    const char* v1[] = {"Alice", "admin"};
    int id1 = table_insert(&users, f1, v1, 2);

    const char* v2[] = {"Bob", "user"};
    table_insert(&users, f1, v2, 2);

    const Row* r = table_find_by_id(&users, id1);
    if (r) {
        printf("\nFound: ");
        for (int i = 0; i < r->field_count; i++)
            printf("%s=%s ", r->fields[i], r->values[i]);
        printf("\n");
    }

    printf("Total rows: %d\n", users.count);
    return 0;
}
