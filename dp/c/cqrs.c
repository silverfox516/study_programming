// CQRS (Command Query Responsibility Segregation) in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS 32
#define MAX_USERS 16

typedef enum { EVT_USER_CREATED, EVT_EMAIL_UPDATED } EventType;

typedef struct {
    EventType type;
    int user_id;
    char data[64];
} Event;

// Write side
typedef struct {
    Event events[MAX_EVENTS];
    int count;
} EventStore;

void cmd_create_user(EventStore* es, int id, const char* name) {
    printf("[Cmd] CreateUser: %s (%d)\n", name, id);
    es->events[es->count++] = (Event){EVT_USER_CREATED, id, ""};
    snprintf(es->events[es->count - 1].data, 64, "%s", name);
}

void cmd_update_email(EventStore* es, int id, const char* email) {
    printf("[Cmd] UpdateEmail: %d -> %s\n", id, email);
    es->events[es->count++] = (Event){EVT_EMAIL_UPDATED, id, ""};
    snprintf(es->events[es->count - 1].data, 64, "%s", email);
}

// Read side
typedef struct { int id; char name[32]; char email[64]; } UserView;

typedef struct { UserView users[MAX_USERS]; int count; } QueryModel;

void query_apply(QueryModel* qm, const Event* e) {
    if (e->type == EVT_USER_CREATED) {
        qm->users[qm->count].id = e->user_id;
        snprintf(qm->users[qm->count].name, 32, "%s", e->data);
        qm->users[qm->count].email[0] = '\0';
        qm->count++;
    } else if (e->type == EVT_EMAIL_UPDATED) {
        for (int i = 0; i < qm->count; i++)
            if (qm->users[i].id == e->user_id)
                snprintf(qm->users[i].email, 64, "%s", e->data);
    }
}

int main(void) {
    EventStore es = {.count = 0};
    QueryModel qm = {.count = 0};

    cmd_create_user(&es, 1, "Alice");
    cmd_create_user(&es, 2, "Bob");
    cmd_update_email(&es, 1, "alice@example.com");

    for (int i = 0; i < es.count; i++)
        query_apply(&qm, &es.events[i]);

    printf("\n--- Query Results ---\n");
    for (int i = 0; i < qm.count; i++)
        printf("  id=%d name=%s email=%s\n", qm.users[i].id, qm.users[i].name, qm.users[i].email);
    return 0;
}
