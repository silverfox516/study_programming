// Specification Pattern in C

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct { char name[32]; double price; char category[16]; } Product;

typedef bool (*SpecFn)(const Product*);

bool is_cheap(const Product* p) { return p->price <= 30.0; }
bool is_toy(const Product* p) { return strcmp(p->category, "toys") == 0; }
bool is_electronics(const Product* p) { return strcmp(p->category, "electronics") == 0; }

// Composite specs via closures aren't native in C, so use struct-based approach
typedef struct Spec {
    enum { SPEC_FN, SPEC_AND, SPEC_OR, SPEC_NOT } type;
    union {
        SpecFn fn;
        struct { const struct Spec* a; const struct Spec* b; } pair;
        const struct Spec* inner;
    };
} Spec;

bool spec_eval(const Spec* s, const Product* p) {
    switch (s->type) {
        case SPEC_FN: return s->fn(p);
        case SPEC_AND: return spec_eval(s->pair.a, p) && spec_eval(s->pair.b, p);
        case SPEC_OR: return spec_eval(s->pair.a, p) || spec_eval(s->pair.b, p);
        case SPEC_NOT: return !spec_eval(s->inner, p);
    }
    return false;
}

#define SPEC_SIMPLE(f) ((Spec){.type = SPEC_FN, .fn = (f)})
#define SPEC_AND_OF(a, b) ((Spec){.type = SPEC_AND, .pair = {&(a), &(b)}})
#define SPEC_NOT_OF(a) ((Spec){.type = SPEC_NOT, .inner = &(a)})

int main(void) {
    Product products[] = {
        {"Toy Car", 15.0, "toys"},
        {"Laptop", 999.0, "electronics"},
        {"Board Game", 25.0, "toys"},
        {"Phone Case", 10.0, "electronics"},
    };
    int n = sizeof(products) / sizeof(products[0]);

    Spec cheap = SPEC_SIMPLE(is_cheap);
    Spec toy = SPEC_SIMPLE(is_toy);
    Spec elec = SPEC_SIMPLE(is_electronics);
    Spec cheap_toys = SPEC_AND_OF(cheap, toy);
    Spec not_elec = SPEC_NOT_OF(elec);

    printf("Cheap products (<=30):\n");
    for (int i = 0; i < n; i++)
        if (spec_eval(&cheap, &products[i]))
            printf("  %s ($%.2f)\n", products[i].name, products[i].price);

    printf("\nCheap toys:\n");
    for (int i = 0; i < n; i++)
        if (spec_eval(&cheap_toys, &products[i]))
            printf("  %s ($%.2f)\n", products[i].name, products[i].price);

    printf("\nNon-electronics:\n");
    for (int i = 0; i < n; i++)
        if (spec_eval(&not_elec, &products[i]))
            printf("  %s\n", products[i].name);
    return 0;
}
