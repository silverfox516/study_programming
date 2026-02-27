# Null Object Pattern

## 개요
Null Object 패턴은 null 참조 대신 기본 동작을 제공하는 객체를 사용하는 행위 패턴입니다. null 검사를 제거하고 기본적인 "아무것도 하지 않는" 동작을 제공합니다.

## C 언어 구현
```c
typedef struct Logger {
    void (*log)(struct Logger* self, const char* message);
    void (*destroy)(struct Logger* self);
} Logger;

// 실제 로거
typedef struct FileLogger {
    Logger base;
    FILE* file;
} FileLogger;

void file_logger_log(Logger* self, const char* message) {
    FileLogger* logger = (FileLogger*)self;
    fprintf(logger->file, "[LOG] %s\n", message);
    fflush(logger->file);
}

FileLogger* create_file_logger(const char* filename) {
    FileLogger* logger = malloc(sizeof(FileLogger));
    logger->file = fopen(filename, "a");
    if (!logger->file) {
        free(logger);
        return NULL;
    }
    logger->base.log = file_logger_log;
    return logger;
}

// Null Object 로거
typedef struct NullLogger {
    Logger base;
} NullLogger;

void null_logger_log(Logger* self, const char* message) {
    // 아무것도 하지 않음
}

void null_logger_destroy(Logger* self) {
    free(self);
}

Logger* create_null_logger() {
    NullLogger* logger = malloc(sizeof(NullLogger));
    logger->base.log = null_logger_log;
    logger->base.destroy = null_logger_destroy;
    return (Logger*)logger;
}

// 사용하는 클래스
typedef struct Service {
    Logger* logger;
} Service;

Service* create_service(Logger* logger) {
    Service* service = malloc(sizeof(Service));
    service->logger = logger ? logger : create_null_logger();  // null이면 Null Object 사용
    return service;
}

void service_do_something(Service* service) {
    service->logger->log(service->logger, "Service is doing something");  // null 검사 불필요
    printf("Service completed its work\n");
}
```

## 사용 예제
```c
// 정상적인 로거 사용
Logger* file_logger = (Logger*)create_file_logger("app.log");
Service* service1 = create_service(file_logger);
service_do_something(service1);  // 파일에 로그 기록

// null 로거 사용 (null 검사 없이 안전)
Service* service2 = create_service(NULL);  // Null Object가 자동으로 사용됨
service_do_something(service2);           // 로그는 기록되지 않지만 에러도 없음
```

## 적용 상황
- **선택적 의존성**: 로깅, 알림 등 있어도 되고 없어도 되는 기능
- **기본 동작**: null 대신 기본 동작을 제공하고 싶을 때
- **테스트**: 실제 객체 없이도 테스트가 가능
- **플러그인 시스템**: 플러그인이 없어도 동작하는 시스템