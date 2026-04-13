// Reactor Pattern in C
// Event demultiplexer dispatches I/O events to registered handlers

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HANDLERS 16

typedef enum { EVT_READ, EVT_WRITE, EVT_CONNECT, EVT_CLOSE } EventType;

typedef struct {
    int fd;
    EventType type;
    char data[128];
} Event;

typedef void (*EventHandler)(const Event*);

typedef struct {
    int fd;
    EventType type;
    EventHandler handler;
} Registration;

typedef struct {
    Registration regs[MAX_HANDLERS];
    int count;
} Reactor;

void reactor_init(Reactor* r) { r->count = 0; }

void reactor_register(Reactor* r, int fd, EventType type, EventHandler handler) {
    if (r->count >= MAX_HANDLERS) return;
    r->regs[r->count++] = (Registration){fd, type, handler};
    printf("[Reactor] Registered fd=%d type=%d\n", fd, type);
}

void reactor_dispatch(Reactor* r, const Event* events, int n) {
    for (int i = 0; i < n; i++) {
        int found = 0;
        for (int j = 0; j < r->count; j++) {
            if (r->regs[j].fd == events[i].fd && r->regs[j].type == events[i].type) {
                r->regs[j].handler(&events[i]);
                found = 1;
                break;
            }
        }
        if (!found)
            printf("[Reactor] No handler for fd=%d type=%d\n", events[i].fd, events[i].type);
    }
}

void on_read(const Event* e) {
    printf("  [Read] fd=%d data='%s'\n", e->fd, e->data);
}
void on_connect(const Event* e) {
    printf("  [Connect] fd=%d from '%s'\n", e->fd, e->data);
}
void on_close(const Event* e) {
    printf("  [Close] fd=%d\n", e->fd);
}

int main(void) {
    Reactor reactor;
    reactor_init(&reactor);

    reactor_register(&reactor, 1, EVT_READ, on_read);
    reactor_register(&reactor, 1, EVT_CLOSE, on_close);
    reactor_register(&reactor, 2, EVT_CONNECT, on_connect);

    Event events[] = {
        {2, EVT_CONNECT, "client:8080"},
        {1, EVT_READ, "GET /index"},
        {1, EVT_READ, "GET /about"},
        {1, EVT_CLOSE, ""},
    };

    printf("\n--- Event Loop ---\n");
    reactor_dispatch(&reactor, events, 4);
    return 0;
}
