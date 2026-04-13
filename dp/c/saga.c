// Saga Pattern in C

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef bool (*StepAction)(void);
typedef void (*StepCompensation)(void);

typedef struct {
    const char* name;
    StepAction action;
    StepCompensation compensate;
} SagaStep;

#define MAX_STEPS 16

typedef struct {
    SagaStep steps[MAX_STEPS];
    int count;
} Saga;

void saga_init(Saga* s) { s->count = 0; }

void saga_add(Saga* s, const char* name, StepAction action, StepCompensation comp) {
    s->steps[s->count++] = (SagaStep){name, action, comp};
}

bool saga_execute(Saga* s) {
    int completed = 0;
    for (int i = 0; i < s->count; i++) {
        printf("[Saga] Executing: %s\n", s->steps[i].name);
        if (s->steps[i].action()) {
            completed++;
        } else {
            printf("[Saga] FAILED at '%s'\n", s->steps[i].name);
            printf("[Saga] Compensating...\n");
            for (int j = completed - 1; j >= 0; j--) {
                printf("[Saga] Compensating: %s\n", s->steps[j].name);
                s->steps[j].compensate();
            }
            return false;
        }
    }
    printf("[Saga] All steps completed.\n");
    return true;
}

// Steps
bool reserve_stock(void) { printf("  Reserved 5 items\n"); return true; }
void release_stock(void) { printf("  Released 5 items\n"); }
bool charge_payment(void) { printf("  Charged $99\n"); return true; }
void refund_payment(void) { printf("  Refunded $99\n"); }
bool ship_order(void) { printf("  Shipped\n"); return true; }
void cancel_ship(void) { printf("  Cancelled shipment\n"); }
bool fail_payment(void) { return false; }

int main(void) {
    printf("=== Success ===\n");
    Saga s1;
    saga_init(&s1);
    saga_add(&s1, "Reserve Stock", reserve_stock, release_stock);
    saga_add(&s1, "Charge Payment", charge_payment, refund_payment);
    saga_add(&s1, "Ship Order", ship_order, cancel_ship);
    saga_execute(&s1);

    printf("\n=== Failure ===\n");
    Saga s2;
    saga_init(&s2);
    saga_add(&s2, "Reserve Stock", reserve_stock, release_stock);
    saga_add(&s2, "Charge Payment", fail_payment, refund_payment);
    saga_add(&s2, "Ship Order", ship_order, cancel_ship);
    saga_execute(&s2);
    return 0;
}
