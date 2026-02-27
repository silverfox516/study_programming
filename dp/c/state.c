#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Forward declaration
typedef struct Context Context;

// State interface
typedef struct State {
    void (*handle_request)(struct State* self, Context* context);
    void (*enter_state)(struct State* self, Context* context);
    void (*exit_state)(struct State* self, Context* context);
    char* (*get_name)(struct State* self);
    void (*destroy)(struct State* self);
} State;

// Context - Vending Machine
typedef struct Context {
    State* current_state;
    float balance;
    float item_price;
    int item_count;
    char* selected_item;
} Context;

Context* context_create();
void context_set_state(Context* context, State* state);
void context_request(Context* context);
void context_insert_money(Context* context, float amount);
void context_select_item(Context* context, const char* item, float price);
void context_dispense_item(Context* context);
void context_return_money(Context* context);
void context_destroy(Context* context);

// Forward declarations for states
State* idle_state_create();
State* has_money_state_create();
State* dispensing_state_create();
State* out_of_stock_state_create();

// Idle State - waiting for money
typedef struct {
    State base;
    char* name;
} IdleState;

void idle_handle_request(State* self, Context* context) {
    printf("Machine is idle. Please insert money to continue.\n");
}

void idle_enter_state(State* self, Context* context) {
    printf("Entering idle state. Balance: $%.2f\n", context->balance);
}

void idle_exit_state(State* self, Context* context) {
    printf("Exiting idle state.\n");
}

char* idle_get_name(State* self) {
    IdleState* state = (IdleState*)self;
    return state->name;
}

void idle_destroy(State* self) {
    IdleState* state = (IdleState*)self;
    if (state->name) {
        free(state->name);
    }
    free(state);
}

State* idle_state_create() {
    IdleState* state = malloc(sizeof(IdleState));
    state->base.handle_request = idle_handle_request;
    state->base.enter_state = idle_enter_state;
    state->base.exit_state = idle_exit_state;
    state->base.get_name = idle_get_name;
    state->base.destroy = idle_destroy;

    state->name = malloc(strlen("Idle") + 1);
    strcpy(state->name, "Idle");

    return (State*)state;
}

// Has Money State - money inserted, waiting for selection
typedef struct {
    State base;
    char* name;
} HasMoneyState;

void has_money_handle_request(State* self, Context* context) {
    if (context->selected_item == NULL) {
        printf("Money inserted. Please select an item.\n");
        return;
    }

    if (context->balance >= context->item_price) {
        if (context->item_count > 0) {
            printf("Item selected and sufficient funds. Transitioning to dispensing.\n");
            context_set_state(context, dispensing_state_create());
        } else {
            printf("Selected item is out of stock.\n");
            context_set_state(context, out_of_stock_state_create());
        }
    } else {
        printf("Insufficient funds. Need $%.2f more.\n", context->item_price - context->balance);
    }
}

void has_money_enter_state(State* self, Context* context) {
    printf("Entering has money state. Balance: $%.2f\n", context->balance);
}

void has_money_exit_state(State* self, Context* context) {
    printf("Exiting has money state.\n");
}

char* has_money_get_name(State* self) {
    HasMoneyState* state = (HasMoneyState*)self;
    return state->name;
}

void has_money_destroy(State* self) {
    HasMoneyState* state = (HasMoneyState*)self;
    if (state->name) {
        free(state->name);
    }
    free(state);
}

State* has_money_state_create() {
    HasMoneyState* state = malloc(sizeof(HasMoneyState));
    state->base.handle_request = has_money_handle_request;
    state->base.enter_state = has_money_enter_state;
    state->base.exit_state = has_money_exit_state;
    state->base.get_name = has_money_get_name;
    state->base.destroy = has_money_destroy;

    state->name = malloc(strlen("HasMoney") + 1);
    strcpy(state->name, "HasMoney");

    return (State*)state;
}

// Dispensing State - dispensing the selected item
typedef struct {
    State base;
    char* name;
} DispensingState;

void dispensing_handle_request(State* self, Context* context) {
    printf("Dispensing %s...\n", context->selected_item);
    context->balance -= context->item_price;
    context->item_count--;

    printf("Item dispensed! Change: $%.2f\n", context->balance);

    if (context->balance > 0) {
        context_return_money(context);
    }

    // Reset selection
    if (context->selected_item) {
        free(context->selected_item);
        context->selected_item = NULL;
    }
    context->item_price = 0.0;

    // Return to idle state
    context_set_state(context, idle_state_create());
}

void dispensing_enter_state(State* self, Context* context) {
    printf("Entering dispensing state.\n");
}

void dispensing_exit_state(State* self, Context* context) {
    printf("Exiting dispensing state.\n");
}

char* dispensing_get_name(State* self) {
    DispensingState* state = (DispensingState*)self;
    return state->name;
}

void dispensing_destroy(State* self) {
    DispensingState* state = (DispensingState*)self;
    if (state->name) {
        free(state->name);
    }
    free(state);
}

State* dispensing_state_create() {
    DispensingState* state = malloc(sizeof(DispensingState));
    state->base.handle_request = dispensing_handle_request;
    state->base.enter_state = dispensing_enter_state;
    state->base.exit_state = dispensing_exit_state;
    state->base.get_name = dispensing_get_name;
    state->base.destroy = dispensing_destroy;

    state->name = malloc(strlen("Dispensing") + 1);
    strcpy(state->name, "Dispensing");

    return (State*)state;
}

// Out of Stock State - selected item is not available
typedef struct {
    State base;
    char* name;
} OutOfStockState;

void out_of_stock_handle_request(State* self, Context* context) {
    printf("Selected item is out of stock. Returning money.\n");
    context_return_money(context);

    // Reset selection
    if (context->selected_item) {
        free(context->selected_item);
        context->selected_item = NULL;
    }
    context->item_price = 0.0;

    // Return to idle state
    context_set_state(context, idle_state_create());
}

void out_of_stock_enter_state(State* self, Context* context) {
    printf("Entering out of stock state.\n");
}

void out_of_stock_exit_state(State* self, Context* context) {
    printf("Exiting out of stock state.\n");
}

char* out_of_stock_get_name(State* self) {
    OutOfStockState* state = (OutOfStockState*)self;
    return state->name;
}

void out_of_stock_destroy(State* self) {
    OutOfStockState* state = (OutOfStockState*)self;
    if (state->name) {
        free(state->name);
    }
    free(state);
}

State* out_of_stock_state_create() {
    OutOfStockState* state = malloc(sizeof(OutOfStockState));
    state->base.handle_request = out_of_stock_handle_request;
    state->base.enter_state = out_of_stock_enter_state;
    state->base.exit_state = out_of_stock_exit_state;
    state->base.get_name = out_of_stock_get_name;
    state->base.destroy = out_of_stock_destroy;

    state->name = malloc(strlen("OutOfStock") + 1);
    strcpy(state->name, "OutOfStock");

    return (State*)state;
}

// Context implementation
Context* context_create() {
    Context* context = malloc(sizeof(Context));
    context->current_state = idle_state_create();
    context->balance = 0.0;
    context->item_price = 0.0;
    context->item_count = 5; // Start with 5 items in stock
    context->selected_item = NULL;

    context->current_state->enter_state(context->current_state, context);
    return context;
}

void context_set_state(Context* context, State* new_state) {
    if (context->current_state) {
        context->current_state->exit_state(context->current_state, context);
        context->current_state->destroy(context->current_state);
    }

    context->current_state = new_state;
    if (new_state) {
        new_state->enter_state(new_state, context);
    }
}

void context_request(Context* context) {
    if (context->current_state) {
        context->current_state->handle_request(context->current_state, context);
    }
}

void context_insert_money(Context* context, float amount) {
    printf("Inserting $%.2f\n", amount);
    context->balance += amount;

    // Transition to has money state if we were idle
    if (strcmp(context->current_state->get_name(context->current_state), "Idle") == 0) {
        context_set_state(context, has_money_state_create());
    }
}

void context_select_item(Context* context, const char* item, float price) {
    printf("Selecting item: %s ($%.2f)\n", item, price);

    if (context->selected_item) {
        free(context->selected_item);
    }

    context->selected_item = malloc(strlen(item) + 1);
    strcpy(context->selected_item, item);
    context->item_price = price;
}

void context_dispense_item(Context* context) {
    context_request(context);
}

void context_return_money(Context* context) {
    if (context->balance > 0) {
        printf("Returning $%.2f\n", context->balance);
        context->balance = 0.0;
    }
}

void context_destroy(Context* context) {
    if (context->current_state) {
        context->current_state->destroy(context->current_state);
    }
    if (context->selected_item) {
        free(context->selected_item);
    }
    free(context);
}

// Client code
int main() {
    printf("=== State Pattern Demo - Vending Machine ===\n\n");

    Context* machine = context_create();

    printf("=== Scenario 1: Successful Purchase ===\n");
    context_request(machine); // Should show idle message

    context_insert_money(machine, 1.50);
    context_select_item(machine, "Soda", 1.25);
    context_dispense_item(machine); // Should dispense and give change

    printf("\n=== Scenario 2: Insufficient Funds ===\n");
    context_insert_money(machine, 0.75);
    context_select_item(machine, "Chips", 1.00);
    context_dispense_item(machine); // Should ask for more money

    context_insert_money(machine, 0.50); // Add more money
    context_dispense_item(machine); // Should now dispense

    printf("\n=== Scenario 3: Out of Stock ===\n");
    // Manually set item count to 0 to simulate out of stock
    machine->item_count = 0;

    context_insert_money(machine, 2.00);
    context_select_item(machine, "Candy", 1.50);
    context_dispense_item(machine); // Should handle out of stock

    printf("\n=== Final State ===\n");
    printf("Current state: %s\n", machine->current_state->get_name(machine->current_state));
    printf("Balance: $%.2f\n", machine->balance);
    printf("Items in stock: %d\n", machine->item_count);

    context_destroy(machine);

    return 0;
}