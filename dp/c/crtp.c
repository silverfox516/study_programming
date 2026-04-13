// CRTP-like Pattern in C
// Static polymorphism via macro-generated functions

#include <stdio.h>

// "CRTP" via macro: generate type-specific functions with shared logic
#define DEFINE_COMPARABLE(Type, field) \
    int Type##_less_than(const Type* a, const Type* b) { return a->field < b->field; } \
    int Type##_greater_than(const Type* a, const Type* b) { return a->field > b->field; } \
    const Type* Type##_clamp(const Type* val, const Type* lo, const Type* hi) { \
        if (val->field < lo->field) return lo; \
        if (val->field > hi->field) return hi; \
        return val; \
    }

typedef struct { int value; } Score;
DEFINE_COMPARABLE(Score, value)

typedef struct { double amount; } Price;
DEFINE_COMPARABLE(Price, amount)

// Mixin via embedding + function pointers
typedef struct Animal {
    const char* name;
    const char* (*sound)(void);
    const char* type_name;
} Animal;

void animal_introduce(const Animal* a) {
    printf("I am %s the %s, I say '%s'\n", a->name, a->type_name, a->sound());
}

const char* dog_sound(void) { return "Woof!"; }
const char* cat_sound(void) { return "Meow!"; }

int main(void) {
    printf("=== Comparable Mixin ===\n");
    Score s1 = {85}, s2 = {92};
    printf("Score 85 < 92: %s\n", Score_less_than(&s1, &s2) ? "true" : "false");

    Score lo = {0}, hi = {100}, over = {150};
    const Score* clamped = Score_clamp(&over, &lo, &hi);
    printf("Score 150 clamped: %d\n", clamped->value);

    Price p1 = {9.99}, p2 = {19.99};
    printf("Price 9.99 > 19.99: %s\n", Price_greater_than(&p1, &p2) ? "true" : "false");

    printf("\n=== Animal Mixin ===\n");
    Animal dog = {"Rex", dog_sound, "Dog"};
    Animal cat = {"Luna", cat_sound, "Cat"};
    animal_introduce(&dog);
    animal_introduce(&cat);
    return 0;
}
