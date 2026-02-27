#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Abstract interface using function pointers
typedef struct Customer {
    char name[100];
    int id;

    // Methods
    void (*greet)(struct Customer* self);
    void (*purchase)(struct Customer* self, const char* item);
    int (*get_discount_rate)(struct Customer* self);
    int (*is_null)(struct Customer* self);
} Customer;

// Real Customer implementation
typedef struct {
    Customer base;
    char email[100];
    int loyalty_points;
} RealCustomer;

void real_customer_greet(Customer* self) {
    printf("Hello %s! Welcome back!\n", self->name);
}

void real_customer_purchase(Customer* self, const char* item) {
    RealCustomer* real = (RealCustomer*)self;
    printf("%s purchased: %s\n", self->name, item);
    real->loyalty_points += 10;
    printf("Loyalty points: %d\n", real->loyalty_points);
}

int real_customer_get_discount_rate(Customer* self) {
    RealCustomer* real = (RealCustomer*)self;
    if (real->loyalty_points > 100) return 15;
    if (real->loyalty_points > 50) return 10;
    return 5;
}

int real_customer_is_null(Customer* self) {
    return 0; // Not null
}

RealCustomer* create_real_customer(int id, const char* name, const char* email) {
    RealCustomer* customer = malloc(sizeof(RealCustomer));
    if (!customer) return NULL;

    customer->base.id = id;
    strncpy(customer->base.name, name, sizeof(customer->base.name) - 1);
    customer->base.name[sizeof(customer->base.name) - 1] = '\0';

    strncpy(customer->email, email, sizeof(customer->email) - 1);
    customer->email[sizeof(customer->email) - 1] = '\0';

    customer->loyalty_points = 0;

    // Set function pointers
    customer->base.greet = real_customer_greet;
    customer->base.purchase = real_customer_purchase;
    customer->base.get_discount_rate = real_customer_get_discount_rate;
    customer->base.is_null = real_customer_is_null;

    return customer;
}

// Null Customer implementation
typedef struct {
    Customer base;
} NullCustomer;

void null_customer_greet(Customer* self) {
    printf("Welcome, guest!\n");
}

void null_customer_purchase(Customer* self, const char* item) {
    printf("Please register to purchase: %s\n", item);
}

int null_customer_get_discount_rate(Customer* self) {
    return 0; // No discount for null customer
}

int null_customer_is_null(Customer* self) {
    return 1; // This is null
}

NullCustomer* create_null_customer() {
    static NullCustomer null_customer = {
        .base = {
            .name = "Guest",
            .id = -1,
            .greet = null_customer_greet,
            .purchase = null_customer_purchase,
            .get_discount_rate = null_customer_get_discount_rate,
            .is_null = null_customer_is_null
        }
    };
    return &null_customer;
}

// Customer repository
typedef struct {
    RealCustomer** customers;
    int count;
    int capacity;
} CustomerRepository;

CustomerRepository* create_customer_repository() {
    CustomerRepository* repo = malloc(sizeof(CustomerRepository));
    if (!repo) return NULL;

    repo->capacity = 10;
    repo->customers = malloc(sizeof(RealCustomer*) * repo->capacity);
    if (!repo->customers) {
        free(repo);
        return NULL;
    }
    repo->count = 0;
    return repo;
}

void customer_repository_add(CustomerRepository* repo, RealCustomer* customer) {
    if (!repo || !customer || repo->count >= repo->capacity) {
        return;
    }
    repo->customers[repo->count++] = customer;
}

Customer* customer_repository_find(CustomerRepository* repo, int id) {
    if (!repo) return (Customer*)create_null_customer();

    for (int i = 0; i < repo->count; i++) {
        if (repo->customers[i]->base.id == id) {
            return (Customer*)repo->customers[i];
        }
    }

    // Return null object instead of NULL
    return (Customer*)create_null_customer();
}

void destroy_customer_repository(CustomerRepository* repo) {
    if (repo) {
        for (int i = 0; i < repo->count; i++) {
            free(repo->customers[i]);
        }
        free(repo->customers);
        free(repo);
    }
}

// Service using customers
typedef struct {
    CustomerRepository* repository;
} CustomerService;

CustomerService* create_customer_service(CustomerRepository* repository) {
    CustomerService* service = malloc(sizeof(CustomerService));
    if (!service) return NULL;
    service->repository = repository;
    return service;
}

void customer_service_process_order(CustomerService* service, int customer_id, const char* item, double price) {
    if (!service || !item) return;

    Customer* customer = customer_repository_find(service->repository, customer_id);

    // No null check needed - null object handles it
    customer->greet(customer);

    int discount = customer->get_discount_rate(customer);
    double final_price = price * (100 - discount) / 100.0;

    printf("Processing order for customer ID %d\n", customer_id);
    printf("Item: %s, Original price: $%.2f\n", item, price);
    printf("Discount: %d%%, Final price: $%.2f\n", discount, final_price);

    customer->purchase(customer, item);

    if (customer->is_null(customer)) {
        printf("Note: This was a guest purchase\n");
    }

    printf("---\n");
}

void destroy_customer_service(CustomerService* service) {
    free(service);
}

// Logger example with Null Object pattern
typedef struct Logger {
    void (*log)(struct Logger* self, const char* level, const char* message);
    void (*destroy)(struct Logger* self);
} Logger;

// Console Logger
typedef struct {
    Logger base;
} ConsoleLogger;

void console_logger_log(Logger* self, const char* level, const char* message) {
    printf("[%s] %s\n", level, message);
}

void console_logger_destroy(Logger* self) {
    free(self);
}

ConsoleLogger* create_console_logger() {
    ConsoleLogger* logger = malloc(sizeof(ConsoleLogger));
    if (!logger) return NULL;

    logger->base.log = console_logger_log;
    logger->base.destroy = console_logger_destroy;

    return logger;
}

// Null Logger
typedef struct {
    Logger base;
} NullLogger;

void null_logger_log(Logger* self, const char* level, const char* message) {
    // Do nothing - silent logging
}

void null_logger_destroy(Logger* self) {
    // Static instance, don't free
}

NullLogger* create_null_logger() {
    static NullLogger null_logger = {
        .base = {
            .log = null_logger_log,
            .destroy = null_logger_destroy
        }
    };
    return &null_logger;
}

// Application using logger
typedef struct {
    Logger* logger;
} Application;

Application* create_application(Logger* logger) {
    Application* app = malloc(sizeof(Application));
    if (!app) return NULL;

    // Use null logger if none provided
    app->logger = logger ? logger : (Logger*)create_null_logger();
    return app;
}

void application_run(Application* app) {
    if (!app) return;

    app->logger->log(app->logger, "INFO", "Application starting");
    app->logger->log(app->logger, "DEBUG", "Processing data");
    app->logger->log(app->logger, "INFO", "Application finished");
}

void destroy_application(Application* app) {
    if (app) {
        free(app);
    }
}

int main() {
    printf("=== Null Object Pattern Demo ===\n");

    // Create repository and add some customers
    CustomerRepository* repo = create_customer_repository();
    if (!repo) {
        printf("Failed to create repository\n");
        return 1;
    }

    // Add real customers
    RealCustomer* alice = create_real_customer(1, "Alice Johnson", "alice@example.com");
    RealCustomer* bob = create_real_customer(2, "Bob Smith", "bob@example.com");

    if (alice) customer_repository_add(repo, alice);
    if (bob) customer_repository_add(repo, bob);

    // Create service
    CustomerService* service = create_customer_service(repo);
    if (!service) {
        printf("Failed to create service\n");
        destroy_customer_repository(repo);
        return 1;
    }

    printf("\n1. Processing orders:\n");

    // Process orders for existing customers
    customer_service_process_order(service, 1, "Laptop", 1000.0);
    customer_service_process_order(service, 2, "Mouse", 50.0);

    // Process order for non-existent customer - null object handles it gracefully
    customer_service_process_order(service, 999, "Keyboard", 100.0);

    printf("\n2. Logger example:\n");

    // Application with console logger
    Logger* console_logger = (Logger*)create_console_logger();
    Application* app_with_logger = create_application(console_logger);

    printf("With console logger:\n");
    application_run(app_with_logger);

    printf("\nWith null logger (silent):\n");
    // Application without logger (uses null logger)
    Application* app_without_logger = create_application(NULL);
    application_run(app_without_logger);

    printf("\nNull Object pattern prevents null pointer exceptions\n");
    printf("and provides default behavior for missing objects.\n");

    // Cleanup
    destroy_customer_service(service);
    destroy_customer_repository(repo);

    console_logger->destroy(console_logger);
    destroy_application(app_with_logger);
    destroy_application(app_without_logger);

    return 0;
}