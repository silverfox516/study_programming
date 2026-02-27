#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 100
#define MAX_EVENTS 1000
#define UUID_LEN 40

// Simple UUID generation (simplified for demo)
static int uuid_counter = 0;

void generate_uuid(char* uuid) {
    snprintf(uuid, UUID_LEN, "UUID-%06d", ++uuid_counter);
}

// Event types enum
typedef enum {
    EVENT_ACCOUNT_OPENED,
    EVENT_MONEY_DEPOSITED,
    EVENT_MONEY_WITHDRAWN,
    EVENT_ACCOUNT_CLOSED
} EventType;

// Base Event structure
typedef struct {
    EventType type;
    char aggregate_id[UUID_LEN];
    time_t timestamp;

    // Event-specific data (union for different event types)
    union {
        struct {
            char owner_name[MAX_NAME_LEN];
            double initial_balance;
        } account_opened;

        struct {
            double amount;
        } money_deposited;

        struct {
            double amount;
        } money_withdrawn;

        struct {
            double final_balance;
        } account_closed;
    } data;
} Event;

// Aggregate (current state)
typedef struct {
    char id[UUID_LEN];
    char owner_name[MAX_NAME_LEN];
    double balance;
    int is_closed;
    int version;
} BankAccount;

// Commands
typedef enum {
    CMD_OPEN_ACCOUNT,
    CMD_DEPOSIT_MONEY,
    CMD_WITHDRAW_MONEY,
    CMD_CLOSE_ACCOUNT
} CommandType;

typedef struct {
    CommandType type;
    char account_id[UUID_LEN];

    union {
        struct {
            char owner_name[MAX_NAME_LEN];
            double initial_balance;
        } open_account;

        struct {
            double amount;
        } deposit_money;

        struct {
            double amount;
        } withdraw_money;
    } data;
} Command;

// Event Store
typedef struct {
    Event events[MAX_EVENTS];
    int event_count;
} EventStore;

// Event utility functions
Event* create_account_opened_event(const char* account_id, const char* owner_name, double initial_balance) {
    Event* event = malloc(sizeof(Event));
    if (!event) return NULL;

    event->type = EVENT_ACCOUNT_OPENED;
    strncpy(event->aggregate_id, account_id, UUID_LEN - 1);
    event->aggregate_id[UUID_LEN - 1] = '\0';
    event->timestamp = time(NULL);

    strncpy(event->data.account_opened.owner_name, owner_name, MAX_NAME_LEN - 1);
    event->data.account_opened.owner_name[MAX_NAME_LEN - 1] = '\0';
    event->data.account_opened.initial_balance = initial_balance;

    return event;
}

Event* create_money_deposited_event(const char* account_id, double amount) {
    Event* event = malloc(sizeof(Event));
    if (!event) return NULL;

    event->type = EVENT_MONEY_DEPOSITED;
    strncpy(event->aggregate_id, account_id, UUID_LEN - 1);
    event->aggregate_id[UUID_LEN - 1] = '\0';
    event->timestamp = time(NULL);
    event->data.money_deposited.amount = amount;

    return event;
}

Event* create_money_withdrawn_event(const char* account_id, double amount) {
    Event* event = malloc(sizeof(Event));
    if (!event) return NULL;

    event->type = EVENT_MONEY_WITHDRAWN;
    strncpy(event->aggregate_id, account_id, UUID_LEN - 1);
    event->aggregate_id[UUID_LEN - 1] = '\0';
    event->timestamp = time(NULL);
    event->data.money_withdrawn.amount = amount;

    return event;
}

Event* create_account_closed_event(const char* account_id, double final_balance) {
    Event* event = malloc(sizeof(Event));
    if (!event) return NULL;

    event->type = EVENT_ACCOUNT_CLOSED;
    strncpy(event->aggregate_id, account_id, UUID_LEN - 1);
    event->aggregate_id[UUID_LEN - 1] = '\0';
    event->timestamp = time(NULL);
    event->data.account_closed.final_balance = final_balance;

    return event;
}

// Event Store functions
EventStore* create_event_store() {
    EventStore* store = malloc(sizeof(EventStore));
    if (!store) return NULL;

    store->event_count = 0;
    return store;
}

int append_event(EventStore* store, const Event* event) {
    if (!store || !event || store->event_count >= MAX_EVENTS) {
        return 0;
    }

    store->events[store->event_count] = *event;
    store->event_count++;
    return 1;
}

int get_events_by_aggregate(EventStore* store, const char* aggregate_id, Event* events, int max_events) {
    if (!store || !aggregate_id || !events) return 0;

    int found = 0;
    for (int i = 0; i < store->event_count && found < max_events; i++) {
        if (strcmp(store->events[i].aggregate_id, aggregate_id) == 0) {
            events[found] = store->events[i];
            found++;
        }
    }
    return found;
}

int get_all_events(EventStore* store, Event* events, int max_events) {
    if (!store || !events) return 0;

    int count = (store->event_count < max_events) ? store->event_count : max_events;
    for (int i = 0; i < count; i++) {
        events[i] = store->events[i];
    }
    return count;
}

void destroy_event_store(EventStore* store) {
    free(store);
}

// Bank Account functions
void init_bank_account(BankAccount* account) {
    if (!account) return;

    memset(account->id, 0, UUID_LEN);
    memset(account->owner_name, 0, MAX_NAME_LEN);
    account->balance = 0.0;
    account->is_closed = 0;
    account->version = 0;
}

void apply_event_to_account(BankAccount* account, const Event* event) {
    if (!account || !event) return;

    switch (event->type) {
        case EVENT_ACCOUNT_OPENED:
            strncpy(account->id, event->aggregate_id, UUID_LEN - 1);
            account->id[UUID_LEN - 1] = '\0';
            strncpy(account->owner_name, event->data.account_opened.owner_name, MAX_NAME_LEN - 1);
            account->owner_name[MAX_NAME_LEN - 1] = '\0';
            account->balance = event->data.account_opened.initial_balance;
            account->is_closed = 0;
            break;

        case EVENT_MONEY_DEPOSITED:
            account->balance += event->data.money_deposited.amount;
            break;

        case EVENT_MONEY_WITHDRAWN:
            account->balance -= event->data.money_withdrawn.amount;
            break;

        case EVENT_ACCOUNT_CLOSED:
            account->is_closed = 1;
            break;
    }
    account->version++;
}

int reconstruct_account_from_events(const Event* events, int event_count, BankAccount* account) {
    if (!events || !account || event_count <= 0) return 0;

    init_bank_account(account);

    for (int i = 0; i < event_count; i++) {
        apply_event_to_account(account, &events[i]);
    }

    return 1;
}

// Command Handler
typedef struct {
    EventStore* event_store;
} CommandHandler;

CommandHandler* create_command_handler(EventStore* event_store) {
    if (!event_store) return NULL;

    CommandHandler* handler = malloc(sizeof(CommandHandler));
    if (!handler) return NULL;

    handler->event_store = event_store;
    return handler;
}

typedef enum {
    RESULT_SUCCESS = 0,
    RESULT_INVALID_COMMAND,
    RESULT_ACCOUNT_EXISTS,
    RESULT_ACCOUNT_NOT_FOUND,
    RESULT_ACCOUNT_CLOSED,
    RESULT_INSUFFICIENT_FUNDS,
    RESULT_INVALID_AMOUNT,
    RESULT_STORAGE_ERROR
} CommandResult;

const char* result_to_string(CommandResult result) {
    switch (result) {
        case RESULT_SUCCESS: return "Success";
        case RESULT_INVALID_COMMAND: return "Invalid command";
        case RESULT_ACCOUNT_EXISTS: return "Account already exists";
        case RESULT_ACCOUNT_NOT_FOUND: return "Account not found";
        case RESULT_ACCOUNT_CLOSED: return "Account is closed";
        case RESULT_INSUFFICIENT_FUNDS: return "Insufficient funds";
        case RESULT_INVALID_AMOUNT: return "Invalid amount";
        case RESULT_STORAGE_ERROR: return "Storage error";
        default: return "Unknown error";
    }
}

CommandResult handle_command(CommandHandler* handler, const Command* command) {
    if (!handler || !command) return RESULT_INVALID_COMMAND;

    switch (command->type) {
        case CMD_OPEN_ACCOUNT: {
            // Validate
            if (command->data.open_account.initial_balance < 0.0) {
                return RESULT_INVALID_AMOUNT;
            }

            // Check if account exists
            Event existing_events[MAX_EVENTS];
            int existing_count = get_events_by_aggregate(handler->event_store,
                                                       command->account_id,
                                                       existing_events,
                                                       MAX_EVENTS);
            if (existing_count > 0) {
                return RESULT_ACCOUNT_EXISTS;
            }

            // Create and store event
            Event* event = create_account_opened_event(command->account_id,
                                                     command->data.open_account.owner_name,
                                                     command->data.open_account.initial_balance);
            if (!event) return RESULT_STORAGE_ERROR;

            int success = append_event(handler->event_store, event);
            free(event);

            return success ? RESULT_SUCCESS : RESULT_STORAGE_ERROR;
        }

        case CMD_DEPOSIT_MONEY: {
            if (command->data.deposit_money.amount <= 0.0) {
                return RESULT_INVALID_AMOUNT;
            }

            // Load current state
            Event events[MAX_EVENTS];
            int event_count = get_events_by_aggregate(handler->event_store,
                                                    command->account_id,
                                                    events,
                                                    MAX_EVENTS);
            if (event_count == 0) {
                return RESULT_ACCOUNT_NOT_FOUND;
            }

            BankAccount account;
            reconstruct_account_from_events(events, event_count, &account);

            if (account.is_closed) {
                return RESULT_ACCOUNT_CLOSED;
            }

            // Create and store event
            Event* event = create_money_deposited_event(command->account_id,
                                                      command->data.deposit_money.amount);
            if (!event) return RESULT_STORAGE_ERROR;

            int success = append_event(handler->event_store, event);
            free(event);

            return success ? RESULT_SUCCESS : RESULT_STORAGE_ERROR;
        }

        case CMD_WITHDRAW_MONEY: {
            if (command->data.withdraw_money.amount <= 0.0) {
                return RESULT_INVALID_AMOUNT;
            }

            // Load current state
            Event events[MAX_EVENTS];
            int event_count = get_events_by_aggregate(handler->event_store,
                                                    command->account_id,
                                                    events,
                                                    MAX_EVENTS);
            if (event_count == 0) {
                return RESULT_ACCOUNT_NOT_FOUND;
            }

            BankAccount account;
            reconstruct_account_from_events(events, event_count, &account);

            if (account.is_closed) {
                return RESULT_ACCOUNT_CLOSED;
            }

            if (account.balance < command->data.withdraw_money.amount) {
                return RESULT_INSUFFICIENT_FUNDS;
            }

            // Create and store event
            Event* event = create_money_withdrawn_event(command->account_id,
                                                      command->data.withdraw_money.amount);
            if (!event) return RESULT_STORAGE_ERROR;

            int success = append_event(handler->event_store, event);
            free(event);

            return success ? RESULT_SUCCESS : RESULT_STORAGE_ERROR;
        }

        case CMD_CLOSE_ACCOUNT: {
            // Load current state
            Event events[MAX_EVENTS];
            int event_count = get_events_by_aggregate(handler->event_store,
                                                    command->account_id,
                                                    events,
                                                    MAX_EVENTS);
            if (event_count == 0) {
                return RESULT_ACCOUNT_NOT_FOUND;
            }

            BankAccount account;
            reconstruct_account_from_events(events, event_count, &account);

            if (account.is_closed) {
                return RESULT_ACCOUNT_CLOSED;
            }

            // Create and store event
            Event* event = create_account_closed_event(command->account_id, account.balance);
            if (!event) return RESULT_STORAGE_ERROR;

            int success = append_event(handler->event_store, event);
            free(event);

            return success ? RESULT_SUCCESS : RESULT_STORAGE_ERROR;
        }

        default:
            return RESULT_INVALID_COMMAND;
    }
}

int get_account_state(CommandHandler* handler, const char* account_id, BankAccount* account) {
    if (!handler || !account_id || !account) return 0;

    Event events[MAX_EVENTS];
    int event_count = get_events_by_aggregate(handler->event_store, account_id, events, MAX_EVENTS);

    if (event_count == 0) return 0;

    return reconstruct_account_from_events(events, event_count, account);
}

void destroy_command_handler(CommandHandler* handler) {
    free(handler);
}

// Read model (projection)
typedef struct {
    char id[UUID_LEN];
    char owner_name[MAX_NAME_LEN];
    double balance;
    double total_deposits;
    double total_withdrawals;
    int transaction_count;
    int is_closed;
} AccountSummary;

void init_account_summary(AccountSummary* summary) {
    if (!summary) return;

    memset(summary->id, 0, UUID_LEN);
    memset(summary->owner_name, 0, MAX_NAME_LEN);
    summary->balance = 0.0;
    summary->total_deposits = 0.0;
    summary->total_withdrawals = 0.0;
    summary->transaction_count = 0;
    summary->is_closed = 0;
}

void create_account_summary_from_events(const Event* events, int event_count, AccountSummary* summary) {
    if (!events || !summary || event_count <= 0) return;

    init_account_summary(summary);

    for (int i = 0; i < event_count; i++) {
        const Event* event = &events[i];

        switch (event->type) {
            case EVENT_ACCOUNT_OPENED:
                strncpy(summary->id, event->aggregate_id, UUID_LEN - 1);
                summary->id[UUID_LEN - 1] = '\0';
                strncpy(summary->owner_name, event->data.account_opened.owner_name, MAX_NAME_LEN - 1);
                summary->owner_name[MAX_NAME_LEN - 1] = '\0';
                summary->balance = event->data.account_opened.initial_balance;
                summary->total_deposits = event->data.account_opened.initial_balance;
                summary->transaction_count++;
                break;

            case EVENT_MONEY_DEPOSITED:
                summary->balance += event->data.money_deposited.amount;
                summary->total_deposits += event->data.money_deposited.amount;
                summary->transaction_count++;
                break;

            case EVENT_MONEY_WITHDRAWN:
                summary->balance -= event->data.money_withdrawn.amount;
                summary->total_withdrawals += event->data.money_withdrawn.amount;
                summary->transaction_count++;
                break;

            case EVENT_ACCOUNT_CLOSED:
                summary->is_closed = 1;
                summary->transaction_count++;
                break;
        }
    }
}

const char* event_type_to_string(EventType type) {
    switch (type) {
        case EVENT_ACCOUNT_OPENED: return "AccountOpened";
        case EVENT_MONEY_DEPOSITED: return "MoneyDeposited";
        case EVENT_MONEY_WITHDRAWN: return "MoneyWithdrawn";
        case EVENT_ACCOUNT_CLOSED: return "AccountClosed";
        default: return "Unknown";
    }
}

void print_event(const Event* event, int event_number) {
    if (!event) return;

    printf("Event %d: %s at %s", event_number, event_type_to_string(event->type), ctime(&event->timestamp));

    switch (event->type) {
        case EVENT_ACCOUNT_OPENED:
            printf("  Account: %s, Owner: %s, Initial Balance: $%.2f\n",
                   event->aggregate_id,
                   event->data.account_opened.owner_name,
                   event->data.account_opened.initial_balance);
            break;

        case EVENT_MONEY_DEPOSITED:
            printf("  Account: %s, Amount: $%.2f\n",
                   event->aggregate_id,
                   event->data.money_deposited.amount);
            break;

        case EVENT_MONEY_WITHDRAWN:
            printf("  Account: %s, Amount: $%.2f\n",
                   event->aggregate_id,
                   event->data.money_withdrawn.amount);
            break;

        case EVENT_ACCOUNT_CLOSED:
            printf("  Account: %s, Final Balance: $%.2f\n",
                   event->aggregate_id,
                   event->data.account_closed.final_balance);
            break;
    }
}

int main() {
    printf("=== Event Sourcing Pattern Demo ===\n\n");

    // Create event store and command handler
    EventStore* event_store = create_event_store();
    if (!event_store) {
        printf("Failed to create event store\n");
        return 1;
    }

    CommandHandler* command_handler = create_command_handler(event_store);
    if (!command_handler) {
        printf("Failed to create command handler\n");
        destroy_event_store(event_store);
        return 1;
    }

    // Create some bank accounts
    char account1_id[UUID_LEN], account2_id[UUID_LEN];
    generate_uuid(account1_id);
    generate_uuid(account2_id);

    printf("1. Opening bank accounts:\n");

    // Open first account
    Command open_cmd1 = {0};
    open_cmd1.type = CMD_OPEN_ACCOUNT;
    strncpy(open_cmd1.account_id, account1_id, UUID_LEN - 1);
    strncpy(open_cmd1.data.open_account.owner_name, "Alice Johnson", MAX_NAME_LEN - 1);
    open_cmd1.data.open_account.initial_balance = 1000.0;

    CommandResult result = handle_command(command_handler, &open_cmd1);
    printf("✓ Account opened for Alice Johnson (ID: %s): %s\n", account1_id, result_to_string(result));

    // Open second account
    Command open_cmd2 = {0};
    open_cmd2.type = CMD_OPEN_ACCOUNT;
    strncpy(open_cmd2.account_id, account2_id, UUID_LEN - 1);
    strncpy(open_cmd2.data.open_account.owner_name, "Bob Smith", MAX_NAME_LEN - 1);
    open_cmd2.data.open_account.initial_balance = 500.0;

    result = handle_command(command_handler, &open_cmd2);
    printf("✓ Account opened for Bob Smith (ID: %s): %s\n", account2_id, result_to_string(result));

    printf("\n2. Performing transactions:\n");

    // Alice deposits money
    Command deposit_cmd = {0};
    deposit_cmd.type = CMD_DEPOSIT_MONEY;
    strncpy(deposit_cmd.account_id, account1_id, UUID_LEN - 1);
    deposit_cmd.data.deposit_money.amount = 250.0;

    result = handle_command(command_handler, &deposit_cmd);
    printf("✓ Alice deposited $250: %s\n", result_to_string(result));

    // Bob withdraws money
    Command withdraw_cmd = {0};
    withdraw_cmd.type = CMD_WITHDRAW_MONEY;
    strncpy(withdraw_cmd.account_id, account2_id, UUID_LEN - 1);
    withdraw_cmd.data.withdraw_money.amount = 100.0;

    result = handle_command(command_handler, &withdraw_cmd);
    printf("✓ Bob withdrew $100: %s\n", result_to_string(result));

    // Alice makes another deposit
    deposit_cmd.data.deposit_money.amount = 500.0;
    result = handle_command(command_handler, &deposit_cmd);
    printf("✓ Alice deposited $500: %s\n", result_to_string(result));

    printf("\n3. Current account states:\n");

    // Show current states
    BankAccount account1, account2;
    if (get_account_state(command_handler, account1_id, &account1)) {
        printf("Alice's account: $%.2f (%s)\n",
               account1.balance,
               account1.is_closed ? "Closed" : "Open");
    }

    if (get_account_state(command_handler, account2_id, &account2)) {
        printf("Bob's account: $%.2f (%s)\n",
               account2.balance,
               account2.is_closed ? "Closed" : "Open");
    }

    printf("\n4. Event history (Alice's account):\n");
    Event alice_events[MAX_EVENTS];
    int alice_event_count = get_events_by_aggregate(event_store, account1_id, alice_events, MAX_EVENTS);

    for (int i = 0; i < alice_event_count; i++) {
        print_event(&alice_events[i], i + 1);
    }

    printf("\n5. Account summary (projection):\n");
    AccountSummary alice_summary;
    create_account_summary_from_events(alice_events, alice_event_count, &alice_summary);

    printf("Alice's Summary:\n");
    printf("  Total deposits: $%.2f\n", alice_summary.total_deposits);
    printf("  Total withdrawals: $%.2f\n", alice_summary.total_withdrawals);
    printf("  Transaction count: %d\n", alice_summary.transaction_count);
    printf("  Current balance: $%.2f\n", alice_summary.balance);

    printf("\n6. Testing business rules:\n");

    // Try to withdraw more than balance
    withdraw_cmd.data.withdraw_money.amount = 1000.0;
    result = handle_command(command_handler, &withdraw_cmd);
    printf("✗ Large withdrawal failed: %s\n", result_to_string(result));

    // Close Bob's account
    Command close_cmd = {0};
    close_cmd.type = CMD_CLOSE_ACCOUNT;
    strncpy(close_cmd.account_id, account2_id, UUID_LEN - 1);

    result = handle_command(command_handler, &close_cmd);
    printf("✓ Bob's account closed: %s\n", result_to_string(result));

    // Try to deposit to closed account
    deposit_cmd.data.deposit_money.amount = 50.0;
    strncpy(deposit_cmd.account_id, account2_id, UUID_LEN - 1);

    result = handle_command(command_handler, &deposit_cmd);
    printf("✗ Deposit to closed account failed: %s\n", result_to_string(result));

    printf("\n7. All events in chronological order:\n");
    Event all_events[MAX_EVENTS];
    int all_event_count = get_all_events(event_store, all_events, MAX_EVENTS);

    for (int i = 0; i < all_event_count; i++) {
        printf("%d. %s - %s (Account: %s)\n",
               i + 1,
               ctime(&all_events[i].timestamp),
               event_type_to_string(all_events[i].type),
               all_events[i].aggregate_id);
    }

    printf("Total events stored: %d\n", event_store->event_count);

    printf("\n✅ Event Sourcing provides complete audit trail and allows\n");
    printf("   reconstruction of state at any point in time!\n");

    // Cleanup
    destroy_command_handler(command_handler);
    destroy_event_store(event_store);

    return 0;
}