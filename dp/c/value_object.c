// Value Object Pattern in C

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct { long amount; char currency[4]; } Money;

Money money_new(long cents, const char* currency) {
    Money m;
    m.amount = cents;
    snprintf(m.currency, 4, "%s", currency);
    return m;
}

Money money_dollars(double d) { return money_new((long)(d * 100), "USD"); }

Money money_add(Money a, Money b) {
    if (strcmp(a.currency, b.currency) != 0) {
        printf("ERROR: cannot add %s + %s\n", a.currency, b.currency);
        return a;
    }
    return money_new(a.amount + b.amount, a.currency);
}

Money money_multiply(Money m, long factor) {
    return money_new(m.amount * factor, m.currency);
}

bool money_equal(Money a, Money b) {
    return a.amount == b.amount && strcmp(a.currency, b.currency) == 0;
}

void money_print(Money m) {
    printf("%ld.%02ld %s", m.amount / 100, m.amount % 100, m.currency);
}

typedef struct { char street[64]; char city[32]; char country[4]; } Address;

bool address_equal(const Address* a, const Address* b) {
    return strcmp(a->street, b->street) == 0 && strcmp(a->city, b->city) == 0
        && strcmp(a->country, b->country) == 0;
}

int main(void) {
    Money p1 = money_dollars(29.99);
    Money p2 = money_dollars(10.01);
    Money total = money_add(p1, p2);

    money_print(p1); printf(" + "); money_print(p2);
    printf(" = "); money_print(total); printf("\n");

    Money a = money_dollars(10.0);
    Money b = money_dollars(10.0);
    printf("$10 == $10: %s\n", money_equal(a, b) ? "true" : "false");

    Money doubled = money_multiply(a, 2);
    printf("Doubled: "); money_print(doubled); printf("\n");

    Address addr1 = {"123 Main St", "Springfield", "US"};
    Address addr2 = {"456 Oak Ave", "Springfield", "US"};
    printf("Same address: %s\n", address_equal(&addr1, &addr2) ? "true" : "false");
    return 0;
}
