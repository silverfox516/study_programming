#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Request types
typedef enum {
    REQUEST_INFO,
    REQUEST_DEBUG,
    REQUEST_WARNING,
    REQUEST_ERROR,
    REQUEST_CRITICAL
} RequestLevel;

typedef struct {
    RequestLevel level;
    char* message;
    char* source;
} LogRequest;

LogRequest* log_request_create(RequestLevel level, const char* message, const char* source) {
    LogRequest* request = malloc(sizeof(LogRequest));
    request->level = level;

    request->message = malloc(strlen(message) + 1);
    strcpy(request->message, message);

    request->source = malloc(strlen(source) + 1);
    strcpy(request->source, source);

    return request;
}

void log_request_destroy(LogRequest* request) {
    if (request->message) {
        free(request->message);
    }
    if (request->source) {
        free(request->source);
    }
    free(request);
}

const char* level_to_string(RequestLevel level) {
    switch (level) {
        case REQUEST_INFO: return "INFO";
        case REQUEST_DEBUG: return "DEBUG";
        case REQUEST_WARNING: return "WARNING";
        case REQUEST_ERROR: return "ERROR";
        case REQUEST_CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

// Handler interface
typedef struct Handler {
    RequestLevel handled_level;
    struct Handler* next_handler;
    void (*handle_request)(struct Handler* self, LogRequest* request);
    void (*set_next)(struct Handler* self, struct Handler* next);
    void (*destroy)(struct Handler* self);
} Handler;

void handler_set_next(Handler* self, Handler* next) {
    self->next_handler = next;
}

// Concrete Handler 1 - Console Logger
typedef struct {
    Handler base;
    char* name;
} ConsoleLogger;

void console_handle_request(Handler* self, LogRequest* request) {
    ConsoleLogger* logger = (ConsoleLogger*)self;

    if (request->level >= self->handled_level) {
        printf("[CONSOLE] %s: [%s] %s (from %s)\n",
               logger->name, level_to_string(request->level),
               request->message, request->source);
    }

    // Pass to next handler in chain
    if (self->next_handler) {
        self->next_handler->handle_request(self->next_handler, request);
    }
}

void console_destroy(Handler* self) {
    ConsoleLogger* logger = (ConsoleLogger*)self;
    if (logger->name) {
        free(logger->name);
    }
    free(logger);
}

Handler* console_logger_create(RequestLevel level, const char* name) {
    ConsoleLogger* logger = malloc(sizeof(ConsoleLogger));
    logger->base.handled_level = level;
    logger->base.next_handler = NULL;
    logger->base.handle_request = console_handle_request;
    logger->base.set_next = handler_set_next;
    logger->base.destroy = console_destroy;

    logger->name = malloc(strlen(name) + 1);
    strcpy(logger->name, name);

    return (Handler*)logger;
}

// Concrete Handler 2 - File Logger
typedef struct {
    Handler base;
    char* filename;
    char* name;
} FileLogger;

void file_handle_request(Handler* self, LogRequest* request) {
    FileLogger* logger = (FileLogger*)self;

    if (request->level >= self->handled_level) {
        printf("[FILE] %s: Writing to '%s': [%s] %s (from %s)\n",
               logger->name, logger->filename, level_to_string(request->level),
               request->message, request->source);

        // In a real implementation, this would write to an actual file
        // FILE* file = fopen(logger->filename, "a");
        // fprintf(file, "[%s] %s: %s\n", level_to_string(request->level), request->source, request->message);
        // fclose(file);
    }

    // Pass to next handler in chain
    if (self->next_handler) {
        self->next_handler->handle_request(self->next_handler, request);
    }
}

void file_destroy(Handler* self) {
    FileLogger* logger = (FileLogger*)self;
    if (logger->filename) {
        free(logger->filename);
    }
    if (logger->name) {
        free(logger->name);
    }
    free(logger);
}

Handler* file_logger_create(RequestLevel level, const char* filename, const char* name) {
    FileLogger* logger = malloc(sizeof(FileLogger));
    logger->base.handled_level = level;
    logger->base.next_handler = NULL;
    logger->base.handle_request = file_handle_request;
    logger->base.set_next = handler_set_next;
    logger->base.destroy = file_destroy;

    logger->filename = malloc(strlen(filename) + 1);
    strcpy(logger->filename, filename);

    logger->name = malloc(strlen(name) + 1);
    strcpy(logger->name, name);

    return (Handler*)logger;
}

// Concrete Handler 3 - Email Logger (for critical errors)
typedef struct {
    Handler base;
    char* email_address;
    char* name;
} EmailLogger;

void email_handle_request(Handler* self, LogRequest* request) {
    EmailLogger* logger = (EmailLogger*)self;

    if (request->level >= self->handled_level) {
        printf("[EMAIL] %s: Sending alert to '%s': [%s] %s (from %s)\n",
               logger->name, logger->email_address, level_to_string(request->level),
               request->message, request->source);

        // In a real implementation, this would send an actual email
        printf("  Subject: Critical Error Alert\n");
        printf("  Body: A critical error occurred in %s: %s\n",
               request->source, request->message);
    }

    // Pass to next handler in chain
    if (self->next_handler) {
        self->next_handler->handle_request(self->next_handler, request);
    }
}

void email_destroy(Handler* self) {
    EmailLogger* logger = (EmailLogger*)self;
    if (logger->email_address) {
        free(logger->email_address);
    }
    if (logger->name) {
        free(logger->name);
    }
    free(logger);
}

Handler* email_logger_create(RequestLevel level, const char* email, const char* name) {
    EmailLogger* logger = malloc(sizeof(EmailLogger));
    logger->base.handled_level = level;
    logger->base.next_handler = NULL;
    logger->base.handle_request = email_handle_request;
    logger->base.set_next = handler_set_next;
    logger->base.destroy = email_destroy;

    logger->email_address = malloc(strlen(email) + 1);
    strcpy(logger->email_address, email);

    logger->name = malloc(strlen(name) + 1);
    strcpy(logger->name, name);

    return (Handler*)logger;
}

// Support ticket system example
typedef struct {
    Handler base;
    char* department;
    int max_priority;
} SupportHandler;

void support_handle_request(Handler* self, LogRequest* request) {
    SupportHandler* handler = (SupportHandler*)self;

    // Convert log level to priority (higher level = higher priority)
    int priority = (int)request->level;

    if (priority <= handler->max_priority) {
        printf("[SUPPORT] %s department: Handling ticket priority %d\n",
               handler->department, priority);
        printf("  Issue: %s (from %s)\n", request->message, request->source);
        printf("  Status: Assigned to %s team\n", handler->department);
        return; // Handle the request, don't pass it further
    }

    // Pass to next handler if we can't handle it
    if (self->next_handler) {
        printf("[SUPPORT] %s department: Cannot handle priority %d, escalating...\n",
               handler->department, priority);
        self->next_handler->handle_request(self->next_handler, request);
    } else {
        printf("[SUPPORT] %s department: No one can handle priority %d!\n",
               handler->department, priority);
    }
}

void support_destroy(Handler* self) {
    SupportHandler* handler = (SupportHandler*)self;
    if (handler->department) {
        free(handler->department);
    }
    free(handler);
}

Handler* support_handler_create(int max_priority, const char* department) {
    SupportHandler* handler = malloc(sizeof(SupportHandler));
    handler->base.handled_level = REQUEST_INFO; // Not used in this example
    handler->base.next_handler = NULL;
    handler->base.handle_request = support_handle_request;
    handler->base.set_next = handler_set_next;
    handler->base.destroy = support_destroy;

    handler->max_priority = max_priority;
    handler->department = malloc(strlen(department) + 1);
    strcpy(handler->department, department);

    return (Handler*)handler;
}

// Client code
int main() {
    printf("=== Chain of Responsibility Pattern Demo ===\n\n");

    // Example 1: Logging Chain
    printf("1. Logging Chain Example:\n");
    printf("Setting up logging chain: Console -> File -> Email\n\n");

    // Create handlers
    Handler* console = console_logger_create(REQUEST_INFO, "ConsoleLogger");
    Handler* file = file_logger_create(REQUEST_WARNING, "error.log", "FileLogger");
    Handler* email = email_logger_create(REQUEST_CRITICAL, "admin@company.com", "EmailLogger");

    // Set up the chain
    console->set_next(console, file);
    file->set_next(file, email);

    // Test different log levels
    LogRequest* info_req = log_request_create(REQUEST_INFO, "Application started", "Main");
    LogRequest* warning_req = log_request_create(REQUEST_WARNING, "Low disk space", "FileSystem");
    LogRequest* error_req = log_request_create(REQUEST_ERROR, "Database connection failed", "Database");
    LogRequest* critical_req = log_request_create(REQUEST_CRITICAL, "Server crashed", "System");

    printf("Sending INFO request:\n");
    console->handle_request(console, info_req);
    printf("\n");

    printf("Sending WARNING request:\n");
    console->handle_request(console, warning_req);
    printf("\n");

    printf("Sending ERROR request:\n");
    console->handle_request(console, error_req);
    printf("\n");

    printf("Sending CRITICAL request:\n");
    console->handle_request(console, critical_req);
    printf("\n");

    // Example 2: Support Ticket Chain
    printf("2. Support Ticket System Example:\n");
    printf("Setting up support chain: Level1 -> Level2 -> Manager\n\n");

    Handler* level1 = support_handler_create(1, "Level1");
    Handler* level2 = support_handler_create(3, "Level2");
    Handler* manager = support_handler_create(4, "Manager");

    level1->set_next(level1, level2);
    level2->set_next(level2, manager);

    LogRequest* simple_req = log_request_create(REQUEST_INFO, "Password reset request", "User Portal");
    LogRequest* complex_req = log_request_create(REQUEST_ERROR, "System integration failure", "API Gateway");
    LogRequest* urgent_req = log_request_create(REQUEST_CRITICAL, "Security breach detected", "Security System");

    printf("Simple request (priority 0):\n");
    level1->handle_request(level1, simple_req);
    printf("\n");

    printf("Complex request (priority 3):\n");
    level1->handle_request(level1, complex_req);
    printf("\n");

    printf("Urgent request (priority 4):\n");
    level1->handle_request(level1, urgent_req);
    printf("\n");

    // Cleanup
    log_request_destroy(info_req);
    log_request_destroy(warning_req);
    log_request_destroy(error_req);
    log_request_destroy(critical_req);
    log_request_destroy(simple_req);
    log_request_destroy(complex_req);
    log_request_destroy(urgent_req);

    console->destroy(console);
    file->destroy(file);
    email->destroy(email);
    level1->destroy(level1);
    level2->destroy(level2);
    manager->destroy(manager);

    return 0;
}