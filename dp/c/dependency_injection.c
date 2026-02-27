#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_LEN 256
#define MAX_FILENAME_LEN 100
#define MAX_EMAIL_LEN 100
#define MAX_DATA_LEN 512
#define MAX_SERVICES 10

// Forward declarations
typedef struct Logger Logger;
typedef struct Database Database;
typedef struct EmailService EmailService;
typedef struct UserService UserService;
typedef struct DIContainer DIContainer;

// Logger interface using function pointers
struct Logger {
    void* impl; // Implementation-specific data
    void (*log)(Logger* self, const char* message);
    const char* (*get_name)(Logger* self);
    void (*destroy)(Logger* self);
};

// Database interface
struct Database {
    void* impl;
    char* (*save)(Database* self, const char* data);
    char* (*find)(Database* self, const char* id);
    int (*is_connected)(Database* self);
    void (*destroy)(Database* self);
};

// Email service interface
struct EmailService {
    void* impl;
    int (*send_email)(EmailService* self, const char* to, const char* subject, const char* body);
    const char* (*get_provider_name)(EmailService* self);
    void (*destroy)(EmailService* self);
};

// Console Logger Implementation
typedef struct {
    Logger base;
} ConsoleLogger;

void console_logger_log(Logger* self, const char* message) {
    printf("[CONSOLE LOG] %s\n", message);
}

const char* console_logger_get_name(Logger* self) {
    return "ConsoleLogger";
}

void console_logger_destroy(Logger* self) {
    free(self);
}

Logger* create_console_logger() {
    ConsoleLogger* logger = malloc(sizeof(ConsoleLogger));
    if (!logger) return NULL;

    logger->base.impl = NULL;
    logger->base.log = console_logger_log;
    logger->base.get_name = console_logger_get_name;
    logger->base.destroy = console_logger_destroy;

    return (Logger*)logger;
}

// File Logger Implementation
typedef struct {
    Logger base;
    char filename[MAX_FILENAME_LEN];
} FileLogger;

void file_logger_log(Logger* self, const char* message) {
    FileLogger* file_logger = (FileLogger*)self;
    printf("[FILE LOG to %s] %s\n", file_logger->filename, message);
}

const char* file_logger_get_name(Logger* self) {
    return "FileLogger";
}

void file_logger_destroy(Logger* self) {
    free(self);
}

Logger* create_file_logger(const char* filename) {
    if (!filename) return NULL;

    FileLogger* logger = malloc(sizeof(FileLogger));
    if (!logger) return NULL;

    strncpy(logger->filename, filename, MAX_FILENAME_LEN - 1);
    logger->filename[MAX_FILENAME_LEN - 1] = '\0';

    logger->base.impl = logger;
    logger->base.log = file_logger_log;
    logger->base.get_name = file_logger_get_name;
    logger->base.destroy = file_logger_destroy;

    return (Logger*)logger;
}

// In-Memory Database Implementation
typedef struct {
    Database base;
    char** keys;
    char** values;
    int count;
    int capacity;
    int next_id;
} InMemoryDatabase;

char* memory_db_save(Database* self, const char* data) {
    InMemoryDatabase* db = (InMemoryDatabase*)self->impl;
    if (!db || !data) return NULL;

    if (db->count >= db->capacity) {
        return NULL; // Database full
    }

    // Generate ID
    char* id = malloc(20);
    if (!id) return NULL;
    snprintf(id, 20, "ID%d", ++db->next_id);

    // Store key
    db->keys[db->count] = malloc(strlen(id) + 1);
    if (!db->keys[db->count]) {
        free(id);
        return NULL;
    }
    strcpy(db->keys[db->count], id);

    // Store value
    db->values[db->count] = malloc(strlen(data) + 1);
    if (!db->values[db->count]) {
        free(db->keys[db->count]);
        free(id);
        return NULL;
    }
    strcpy(db->values[db->count], data);

    db->count++;
    return id;
}

char* memory_db_find(Database* self, const char* id) {
    InMemoryDatabase* db = (InMemoryDatabase*)self->impl;
    if (!db || !id) return NULL;

    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->keys[i], id) == 0) {
            char* result = malloc(strlen(db->values[i]) + 1);
            if (result) {
                strcpy(result, db->values[i]);
            }
            return result;
        }
    }
    return NULL;
}

int memory_db_is_connected(Database* self) {
    return self && self->impl ? 1 : 0;
}

void memory_db_destroy(Database* self) {
    if (!self) return;

    InMemoryDatabase* db = (InMemoryDatabase*)self->impl;
    if (db) {
        for (int i = 0; i < db->count; i++) {
            free(db->keys[i]);
            free(db->values[i]);
        }
        free(db->keys);
        free(db->values);
        free(db);
    }
    free(self);
}

Database* create_memory_database(int capacity) {
    Database* database = malloc(sizeof(Database));
    if (!database) return NULL;

    InMemoryDatabase* impl = malloc(sizeof(InMemoryDatabase));
    if (!impl) {
        free(database);
        return NULL;
    }

    impl->capacity = capacity > 0 ? capacity : 10;
    impl->keys = calloc(impl->capacity, sizeof(char*));
    impl->values = calloc(impl->capacity, sizeof(char*));
    impl->count = 0;
    impl->next_id = 0;

    if (!impl->keys || !impl->values) {
        free(impl->keys);
        free(impl->values);
        free(impl);
        free(database);
        return NULL;
    }

    database->impl = impl;
    database->save = memory_db_save;
    database->find = memory_db_find;
    database->is_connected = memory_db_is_connected;
    database->destroy = memory_db_destroy;

    return database;
}

// Mock Email Service Implementation
typedef struct {
    EmailService base;
} MockEmailService;

int mock_email_send(EmailService* self, const char* to, const char* subject, const char* body) {
    printf("[MOCK EMAIL] To: %s, Subject: %s, Body: %s\n", to, subject, body);
    return 1; // Success
}

const char* mock_email_get_provider_name(EmailService* self) {
    return "MockEmailService";
}

void mock_email_destroy(EmailService* self) {
    free(self);
}

EmailService* create_mock_email_service() {
    MockEmailService* service = malloc(sizeof(MockEmailService));
    if (!service) return NULL;

    service->base.impl = NULL;
    service->base.send_email = mock_email_send;
    service->base.get_provider_name = mock_email_get_provider_name;
    service->base.destroy = mock_email_destroy;

    return (EmailService*)service;
}

// SMTP Email Service Implementation
typedef struct {
    EmailService base;
    char server[MAX_FILENAME_LEN];
} SMTPEmailService;

int smtp_email_send(EmailService* self, const char* to, const char* subject, const char* body) {
    SMTPEmailService* smtp = (SMTPEmailService*)self;
    printf("[SMTP via %s] To: %s, Subject: %s, Body: %s\n", smtp->server, to, subject, body);
    return 1;
}

const char* smtp_email_get_provider_name(EmailService* self) {
    return "SMTPEmailService";
}

void smtp_email_destroy(EmailService* self) {
    free(self);
}

EmailService* create_smtp_email_service(const char* server) {
    if (!server) return NULL;

    SMTPEmailService* service = malloc(sizeof(SMTPEmailService));
    if (!service) return NULL;

    strncpy(service->server, server, MAX_FILENAME_LEN - 1);
    service->server[MAX_FILENAME_LEN - 1] = '\0';

    service->base.impl = service;
    service->base.send_email = smtp_email_send;
    service->base.get_provider_name = smtp_email_get_provider_name;
    service->base.destroy = smtp_email_destroy;

    return (EmailService*)service;
}

// User Service (depends on other services)
struct UserService {
    Logger* logger;
    Database* database;
    EmailService* email_service;
};

UserService* create_user_service(Logger* logger, Database* database, EmailService* email_service) {
    if (!logger || !database || !email_service) {
        return NULL;
    }

    UserService* service = malloc(sizeof(UserService));
    if (!service) return NULL;

    service->logger = logger;
    service->database = database;
    service->email_service = email_service;

    return service;
}

char* user_service_create_user(UserService* service, const char* name, const char* email) {
    if (!service || !name || !email) return NULL;

    // Log the operation
    char log_message[MAX_MESSAGE_LEN];
    snprintf(log_message, MAX_MESSAGE_LEN, "Creating user: %s", name);
    service->logger->log(service->logger, log_message);

    // Check database connection
    if (!service->database->is_connected(service->database)) {
        service->logger->log(service->logger, "Database connection failed");
        return NULL;
    }

    // Create user data
    char user_data[MAX_DATA_LEN];
    snprintf(user_data, MAX_DATA_LEN, "{\"name\":\"%s\",\"email\":\"%s\"}", name, email);

    // Save to database
    char* user_id = service->database->save(service->database, user_data);
    if (!user_id) {
        service->logger->log(service->logger, "Failed to save user to database");
        return NULL;
    }

    // Log success
    snprintf(log_message, MAX_MESSAGE_LEN, "User created with ID: %s", user_id);
    service->logger->log(service->logger, log_message);

    // Send welcome email
    char subject[] = "Welcome!";
    char body[MAX_MESSAGE_LEN];
    snprintf(body, MAX_MESSAGE_LEN, "Hello %s, welcome to our service!", name);
    service->email_service->send_email(service->email_service, email, subject, body);

    return user_id;
}

char* user_service_get_user(UserService* service, const char* id) {
    if (!service || !id) return NULL;

    char log_message[MAX_MESSAGE_LEN];
    snprintf(log_message, MAX_MESSAGE_LEN, "Retrieving user with ID: %s", id);
    service->logger->log(service->logger, log_message);

    return service->database->find(service->database, id);
}

void user_service_print_info(UserService* service) {
    if (!service) return;

    printf("UserService Configuration:\n");
    printf("  Logger: %s\n", service->logger->get_name(service->logger));
    printf("  Database: Connected=%s\n", service->database->is_connected(service->database) ? "Yes" : "No");
    printf("  Email: %s\n", service->email_service->get_provider_name(service->email_service));
}

void destroy_user_service(UserService* service) {
    // Note: We don't destroy the dependencies here as they might be shared
    // The dependencies should be managed by the creator/container
    free(service);
}

// Simple Dependency Injection Container
typedef struct {
    char* name;
    void* service;
    void (*destructor)(void*);
} ServiceEntry;

struct DIContainer {
    ServiceEntry services[MAX_SERVICES];
    int count;
};

DIContainer* create_di_container() {
    DIContainer* container = malloc(sizeof(DIContainer));
    if (!container) return NULL;

    container->count = 0;
    return container;
}

int di_container_register(DIContainer* container, const char* name, void* service, void (*destructor)(void*)) {
    if (!container || !name || !service || container->count >= MAX_SERVICES) {
        return 0;
    }

    ServiceEntry* entry = &container->services[container->count];
    entry->name = malloc(strlen(name) + 1);
    if (!entry->name) return 0;

    strcpy(entry->name, name);
    entry->service = service;
    entry->destructor = destructor;

    container->count++;
    return 1;
}

void* di_container_resolve(DIContainer* container, const char* name) {
    if (!container || !name) return NULL;

    for (int i = 0; i < container->count; i++) {
        if (strcmp(container->services[i].name, name) == 0) {
            return container->services[i].service;
        }
    }
    return NULL;
}

void destroy_di_container(DIContainer* container) {
    if (!container) return;

    for (int i = 0; i < container->count; i++) {
        free(container->services[i].name);
        if (container->services[i].destructor) {
            container->services[i].destructor(container->services[i].service);
        }
    }
    free(container);
}

// Factory functions for different configurations
UserService* create_development_user_service() {
    Logger* logger = create_console_logger();
    Database* database = create_memory_database(100);
    EmailService* email_service = create_mock_email_service();

    if (!logger || !database || !email_service) {
        if (logger) logger->destroy(logger);
        if (database) database->destroy(database);
        if (email_service) email_service->destroy(email_service);
        return NULL;
    }

    return create_user_service(logger, database, email_service);
}

UserService* create_production_user_service() {
    Logger* logger = create_file_logger("production.log");
    Database* database = create_memory_database(1000); // In production, would be PostgreSQL
    EmailService* email_service = create_smtp_email_service("smtp.company.com");

    if (!logger || !database || !email_service) {
        if (logger) logger->destroy(logger);
        if (database) database->destroy(database);
        if (email_service) email_service->destroy(email_service);
        return NULL;
    }

    return create_user_service(logger, database, email_service);
}

// Helper function to destroy a complete user service and its dependencies
void destroy_complete_user_service(UserService* service) {
    if (!service) return;

    service->logger->destroy(service->logger);
    service->database->destroy(service->database);
    service->email_service->destroy(service->email_service);
    destroy_user_service(service);
}

int main() {
    printf("=== Dependency Injection Pattern Demo ===\n\n");

    // Method 1: Manual dependency injection
    printf("1. Manual Dependency Injection:\n");
    Logger* logger = create_console_logger();
    Database* database = create_memory_database(10);
    EmailService* email_service = create_mock_email_service();

    if (!logger || !database || !email_service) {
        printf("Failed to create dependencies\n");
        return 1;
    }

    UserService* user_service = create_user_service(logger, database, email_service);
    if (!user_service) {
        printf("Failed to create user service\n");
        return 1;
    }

    user_service_print_info(user_service);

    char* user_id = user_service_create_user(user_service, "Alice Johnson", "alice@example.com");
    if (user_id) {
        printf("User created successfully with ID: %s\n", user_id);

        char* user_data = user_service_get_user(user_service, user_id);
        if (user_data) {
            printf("Retrieved user data: %s\n", user_data);
            free(user_data);
        }
        free(user_id);
    }

    // Cleanup
    destroy_complete_user_service(user_service);

    printf("\n");

    // Method 2: Using factory functions
    printf("2. Factory Functions for DI:\n");

    printf("Development Service:\n");
    UserService* dev_service = create_development_user_service();
    if (dev_service) {
        user_service_print_info(dev_service);
        char* dev_user_id = user_service_create_user(dev_service, "Charlie Brown", "charlie@example.com");
        if (dev_user_id) {
            printf("Dev user created with ID: %s\n", dev_user_id);
            free(dev_user_id);
        }
        destroy_complete_user_service(dev_service);
    }

    printf("\nProduction Service:\n");
    UserService* prod_service = create_production_user_service();
    if (prod_service) {
        user_service_print_info(prod_service);
        char* prod_user_id = user_service_create_user(prod_service, "Diana Prince", "diana@example.com");
        if (prod_user_id) {
            printf("Prod user created with ID: %s\n", prod_user_id);
            free(prod_user_id);
        }
        destroy_complete_user_service(prod_service);
    }

    printf("\n");

    // Method 3: DI Container
    printf("3. DI Container:\n");

    DIContainer* container = create_di_container();
    if (!container) {
        printf("Failed to create DI container\n");
        return 1;
    }

    // Register services
    Logger* container_logger = create_console_logger();
    Database* container_db = create_memory_database(50);
    EmailService* container_email = create_smtp_email_service("smtp.example.com");

    if (!container_logger || !container_db || !container_email) {
        printf("Failed to create services for container\n");
        destroy_di_container(container);
        return 1;
    }

    di_container_register(container, "logger", container_logger, (void(*)(void*))console_logger_destroy);
    di_container_register(container, "database", container_db, (void(*)(void*))memory_db_destroy);
    di_container_register(container, "email", container_email, (void(*)(void*))smtp_email_destroy);

    // Resolve services from container
    Logger* resolved_logger = (Logger*)di_container_resolve(container, "logger");
    Database* resolved_db = (Database*)di_container_resolve(container, "database");
    EmailService* resolved_email = (EmailService*)di_container_resolve(container, "email");

    if (resolved_logger && resolved_db && resolved_email) {
        printf("Services resolved from container successfully\n");
        UserService* container_service = create_user_service(resolved_logger, resolved_db, resolved_email);
        if (container_service) {
            user_service_print_info(container_service);
            char* container_user_id = user_service_create_user(container_service, "Eve Adams", "eve@example.com");
            if (container_user_id) {
                printf("Container user created with ID: %s\n", container_user_id);
                free(container_user_id);
            }
            destroy_user_service(container_service); // Don't destroy dependencies, container will handle it
        }
    }

    // Container will destroy all registered services
    destroy_di_container(container);

    printf("\n✅ Dependency Injection provides flexibility, testability,\n");
    printf("   and loose coupling between components in C!\n");

    return 0;
}