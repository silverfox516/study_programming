#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Coffee {
    char* (*get_description)(struct Coffee* self);
    double (*cost)(struct Coffee* self);
    void (*destroy)(struct Coffee* self);
} Coffee;

// Base coffee implementation
typedef struct SimpleCoffee {
    Coffee base;
} SimpleCoffee;

char* simple_coffee_description(Coffee* self) {
    static char desc[] = "Simple Coffee";
    return desc;
}

double simple_coffee_cost(Coffee* self) {
    return 2.0;
}

void simple_coffee_destroy(Coffee* self) {
    free(self);
}

Coffee* create_simple_coffee() {
    SimpleCoffee* coffee = malloc(sizeof(SimpleCoffee));
    coffee->base.get_description = simple_coffee_description;
    coffee->base.cost = simple_coffee_cost;
    coffee->base.destroy = simple_coffee_destroy;
    return (Coffee*)coffee;
}

// Milk decorator
typedef struct MilkDecorator {
    Coffee base;
    Coffee* coffee;
    char* description_buffer;
} MilkDecorator;

char* milk_description(Coffee* self) {
    MilkDecorator* milk = (MilkDecorator*)self;
    if (milk->description_buffer) {
        free(milk->description_buffer);
    }
    char* base_desc = milk->coffee->get_description(milk->coffee);
    milk->description_buffer = malloc(strlen(base_desc) + 20);
    sprintf(milk->description_buffer, "%s, Milk", base_desc);
    return milk->description_buffer;
}

double milk_cost(Coffee* self) {
    MilkDecorator* milk = (MilkDecorator*)self;
    return milk->coffee->cost(milk->coffee) + 0.5;
}

void milk_destroy(Coffee* self) {
    MilkDecorator* milk = (MilkDecorator*)self;
    if (milk->description_buffer) {
        free(milk->description_buffer);
    }
    milk->coffee->destroy(milk->coffee);
    free(milk);
}

Coffee* add_milk(Coffee* coffee) {
    MilkDecorator* milk = malloc(sizeof(MilkDecorator));
    milk->base.get_description = milk_description;
    milk->base.cost = milk_cost;
    milk->base.destroy = milk_destroy;
    milk->coffee = coffee;
    milk->description_buffer = NULL;
    return (Coffee*)milk;
}

// Sugar decorator
typedef struct SugarDecorator {
    Coffee base;
    Coffee* coffee;
    char* description_buffer;
} SugarDecorator;

char* sugar_description(Coffee* self) {
    SugarDecorator* sugar = (SugarDecorator*)self;
    if (sugar->description_buffer) {
        free(sugar->description_buffer);
    }
    char* base_desc = sugar->coffee->get_description(sugar->coffee);
    sugar->description_buffer = malloc(strlen(base_desc) + 20);
    sprintf(sugar->description_buffer, "%s, Sugar", base_desc);
    return sugar->description_buffer;
}

double sugar_cost(Coffee* self) {
    SugarDecorator* sugar = (SugarDecorator*)self;
    return sugar->coffee->cost(sugar->coffee) + 0.2;
}

void sugar_destroy(Coffee* self) {
    SugarDecorator* sugar = (SugarDecorator*)self;
    if (sugar->description_buffer) {
        free(sugar->description_buffer);
    }
    sugar->coffee->destroy(sugar->coffee);
    free(sugar);
}

Coffee* add_sugar(Coffee* coffee) {
    SugarDecorator* sugar = malloc(sizeof(SugarDecorator));
    sugar->base.get_description = sugar_description;
    sugar->base.cost = sugar_cost;
    sugar->base.destroy = sugar_destroy;
    sugar->coffee = coffee;
    sugar->description_buffer = NULL;
    return (Coffee*)sugar;
}

// Whip decorator
typedef struct WhipDecorator {
    Coffee base;
    Coffee* coffee;
    char* description_buffer;
} WhipDecorator;

char* whip_description(Coffee* self) {
    WhipDecorator* whip = (WhipDecorator*)self;
    if (whip->description_buffer) {
        free(whip->description_buffer);
    }
    char* base_desc = whip->coffee->get_description(whip->coffee);
    whip->description_buffer = malloc(strlen(base_desc) + 20);
    sprintf(whip->description_buffer, "%s, Whip", base_desc);
    return whip->description_buffer;
}

double whip_cost(Coffee* self) {
    WhipDecorator* whip = (WhipDecorator*)self;
    return whip->coffee->cost(whip->coffee) + 0.7;
}

void whip_destroy(Coffee* self) {
    WhipDecorator* whip = (WhipDecorator*)self;
    if (whip->description_buffer) {
        free(whip->description_buffer);
    }
    whip->coffee->destroy(whip->coffee);
    free(whip);
}

Coffee* add_whip(Coffee* coffee) {
    WhipDecorator* whip = malloc(sizeof(WhipDecorator));
    whip->base.get_description = whip_description;
    whip->base.cost = whip_cost;
    whip->base.destroy = whip_destroy;
    whip->coffee = coffee;
    whip->description_buffer = NULL;
    return (Coffee*)whip;
}

int main() {
    printf("--- Coffee Shop with Decorator Pattern ---\n");

    // Simple coffee
    Coffee* coffee1 = create_simple_coffee();
    printf("%s: $%.2f\n", coffee1->get_description(coffee1), coffee1->cost(coffee1));
    coffee1->destroy(coffee1);

    // Coffee with milk
    Coffee* coffee2 = add_milk(create_simple_coffee());
    printf("%s: $%.2f\n", coffee2->get_description(coffee2), coffee2->cost(coffee2));
    coffee2->destroy(coffee2);

    // Coffee with milk and sugar
    Coffee* coffee3 = add_sugar(add_milk(create_simple_coffee()));
    printf("%s: $%.2f\n", coffee3->get_description(coffee3), coffee3->cost(coffee3));
    coffee3->destroy(coffee3);

    // Fully loaded coffee
    Coffee* coffee4 = add_whip(add_sugar(add_milk(create_simple_coffee())));
    printf("%s: $%.2f\n", coffee4->get_description(coffee4), coffee4->cost(coffee4));
    coffee4->destroy(coffee4);

    return 0;
}