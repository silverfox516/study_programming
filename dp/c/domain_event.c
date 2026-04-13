// Domain Event Pattern in C

#include <stdio.h>
#include <string.h>

typedef enum { EVT_ORDER_PLACED, EVT_ORDER_SHIPPED, EVT_PAYMENT_RECEIVED } DomainEventType;

typedef struct {
    DomainEventType type;
    int order_id;
    char data[64];
    double amount;
} DomainEvent;

typedef void (*EventHandler)(const DomainEvent*);

#define MAX_HANDLERS 16
typedef struct {
    EventHandler handlers[MAX_HANDLERS];
    int count;
} EventBus;

void bus_init(EventBus* b) { b->count = 0; }
void bus_subscribe(EventBus* b, EventHandler h) { b->handlers[b->count++] = h; }

void bus_publish(EventBus* b, const DomainEvent* e) {
    for (int i = 0; i < b->count; i++)
        b->handlers[i](e);
}

void email_handler(const DomainEvent* e) {
    if (e->type == EVT_ORDER_PLACED)
        printf("  [Email] Confirmation for order #%d\n", e->order_id);
}

void accounting_handler(const DomainEvent* e) {
    if (e->type == EVT_PAYMENT_RECEIVED)
        printf("  [Accounting] $%.2f for order #%d\n", e->amount, e->order_id);
}

void audit_handler(const DomainEvent* e) {
    const char* names[] = {"OrderPlaced", "OrderShipped", "PaymentReceived"};
    printf("  [Audit] %s #%d\n", names[e->type], e->order_id);
}

int main(void) {
    EventBus bus;
    bus_init(&bus);
    bus_subscribe(&bus, email_handler);
    bus_subscribe(&bus, accounting_handler);
    bus_subscribe(&bus, audit_handler);

    DomainEvent e1 = {EVT_ORDER_PLACED, 1001, "Alice", 99.99};
    printf("Publishing OrderPlaced:\n");
    bus_publish(&bus, &e1);

    DomainEvent e2 = {EVT_PAYMENT_RECEIVED, 1001, "", 99.99};
    printf("\nPublishing PaymentReceived:\n");
    bus_publish(&bus, &e2);
    return 0;
}
