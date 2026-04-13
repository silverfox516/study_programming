// Transaction Script Pattern in C

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ACCOUNTS 16

typedef struct { int id; char name[32]; double balance; } Account;
typedef struct { Account accounts[MAX_ACCOUNTS]; int count; } Store;

void store_add(Store* s, int id, const char* name, double bal) {
    Account* a = &s->accounts[s->count++];
    a->id = id;
    snprintf(a->name, 32, "%s", name);
    a->balance = bal;
}

Account* store_find(Store* s, int id) {
    for (int i = 0; i < s->count; i++)
        if (s->accounts[i].id == id) return &s->accounts[i];
    return NULL;
}

// Transaction scripts
bool transfer(Store* s, int from, int to, double amount) {
    printf("[Transfer] $%.2f from #%d to #%d\n", amount, from, to);
    Account* f = store_find(s, from);
    Account* t = store_find(s, to);
    if (!f || !t) { printf("  Account not found\n"); return false; }
    if (f->balance < amount) { printf("  Insufficient funds\n"); return false; }
    f->balance -= amount;
    t->balance += amount;
    printf("  Success\n");
    return true;
}

void apply_interest(Store* s, double rate) {
    printf("[Interest] %.1f%%\n", rate * 100);
    for (int i = 0; i < s->count; i++) {
        double interest = s->accounts[i].balance * rate;
        s->accounts[i].balance += interest;
        printf("  #%d %s: +%.2f -> %.2f\n",
            s->accounts[i].id, s->accounts[i].name, interest, s->accounts[i].balance);
    }
}

void print_balances(Store* s) {
    printf("\n--- Balances ---\n");
    for (int i = 0; i < s->count; i++)
        printf("  #%d %s: $%.2f\n", s->accounts[i].id, s->accounts[i].name, s->accounts[i].balance);
    printf("\n");
}

int main(void) {
    Store store = {.count = 0};
    store_add(&store, 1, "Alice", 1000.0);
    store_add(&store, 2, "Bob", 500.0);

    print_balances(&store);
    transfer(&store, 1, 2, 200.0);
    print_balances(&store);
    apply_interest(&store, 0.05);
    print_balances(&store);
    transfer(&store, 2, 1, 9999.0); // fails
    return 0;
}
