#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Logger {
    char* log_file;
    FILE* file_handle;
} Logger;

static Logger* instance = NULL;

Logger* get_logger_instance() {
    if (instance == NULL) {
        instance = malloc(sizeof(Logger));
        instance->log_file = "app.log";
        instance->file_handle = fopen(instance->log_file, "a");
        if (!instance->file_handle) {
            fprintf(stderr, "Failed to open log file\n");
            free(instance);
            instance = NULL;
        }
    }
    return instance;
}

void log_message(const char* message) {
    Logger* logger = get_logger_instance();
    if (logger && logger->file_handle) {
        fprintf(logger->file_handle, "[LOG] %s\n", message);
        fflush(logger->file_handle);
    }
}

void cleanup_logger() {
    if (instance) {
        if (instance->file_handle) {
            fclose(instance->file_handle);
        }
        free(instance);
        instance = NULL;
    }
}

int main() {
    log_message("Application started");
    log_message("Performing some operations");
    log_message("Application finished");

    cleanup_logger();
    return 0;
}