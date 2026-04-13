// Compensating Transaction Pattern in C

#include <stdio.h>
#include <stdbool.h>

typedef bool (*Action)(void);
typedef void (*Compensation)(void);

typedef struct { const char* name; Action action; Compensation compensate; } Step;

#define MAX_STEPS 16

bool execute_transaction(Step* steps, int n) {
    int completed = 0;
    for (int i = 0; i < n; i++) {
        printf("[TX] Executing: %s\n", steps[i].name);
        if (steps[i].action()) {
            completed++;
        } else {
            printf("[TX] FAILED at '%s'\n", steps[i].name);
            for (int j = completed - 1; j >= 0; j--) {
                printf("[TX] Compensating: %s\n", steps[j].name);
                steps[j].compensate();
            }
            return false;
        }
    }
    printf("[TX] All steps completed.\n");
    return true;
}

bool debit(void) { printf("  Debited $100\n"); return true; }
void credit_back(void) { printf("  Credited $100 back\n"); }
bool reserve(void) { printf("  Reserved 2 items\n"); return true; }
void release(void) { printf("  Released items\n"); }
bool ship(void) { printf("  Shipped\n"); return true; }
void cancel_ship(void) { printf("  Cancelled shipment\n"); }
bool fail_pay(void) { return false; }
void refund(void) { printf("  Refunded\n"); }

int main(void) {
    printf("=== Success ===\n");
    Step s1[] = {
        {"Debit", debit, credit_back},
        {"Reserve", reserve, release},
        {"Ship", ship, cancel_ship},
    };
    execute_transaction(s1, 3);

    printf("\n=== Failure ===\n");
    Step s2[] = {
        {"Debit", debit, credit_back},
        {"Payment", fail_pay, refund},
        {"Ship", ship, cancel_ship},
    };
    execute_transaction(s2, 3);
    return 0;
}
