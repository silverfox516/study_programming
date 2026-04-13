// Data Mapper Pattern in C

#include <stdio.h>
#include <string.h>

typedef struct { int id; char name[32]; char email[64]; } User;
typedef struct { int user_id; char user_name[32]; char user_email[64]; char created_at[20]; } UserRow;

UserRow user_to_row(const User* u) {
    UserRow r;
    r.user_id = u->id;
    snprintf(r.user_name, 32, "%s", u->name);
    snprintf(r.user_email, 64, "%s", u->email);
    snprintf(r.created_at, 20, "2024-01-01");
    return r;
}

User row_to_user(const UserRow* r) {
    User u;
    u.id = r->user_id;
    snprintf(u.name, 32, "%s", r->user_name);
    snprintf(u.email, 64, "%s", r->user_email);
    return u;
}

#define MAX_ROWS 16
typedef struct { UserRow rows[MAX_ROWS]; int count; } Database;

void db_save(Database* db, const User* u) {
    UserRow r = user_to_row(u);
    printf("[DB] Save: id=%d name=%s\n", r.user_id, r.user_name);
    db->rows[db->count++] = r;
}

User* db_find(Database* db, int id, User* out) {
    for (int i = 0; i < db->count; i++)
        if (db->rows[i].user_id == id) { *out = row_to_user(&db->rows[i]); return out; }
    return NULL;
}

int main(void) {
    Database db = {.count = 0};
    User alice = {1, "Alice", "alice@example.com"};
    User bob = {2, "Bob", "bob@example.com"};
    db_save(&db, &alice);
    db_save(&db, &bob);

    User found;
    if (db_find(&db, 1, &found))
        printf("Found: id=%d name=%s email=%s\n", found.id, found.name, found.email);
    return 0;
}
