# Dependency Injection Pattern

## 개요
Dependency Injection 패턴은 객체가 필요로 하는 의존성을 외부에서 주입하는 디자인 패턴입니다. 객체 내부에서 의존성을 직접 생성하는 대신 외부에서 제공함으로써 결합도를 낮추고 테스트 가능성을 높입니다.

## 구조
- **Service**: 다른 객체들이 의존하는 서비스나 컴포넌트
- **Client**: 서비스를 사용하는 클라이언트 객체
- **Injector**: 의존성을 주입하는 역할을 하는 컴포넌트
- **Interface**: 서비스와 클라이언트 사이의 계약

## C 언어 구현

### 서비스 인터페이스 정의
```c
// Logger 인터페이스
typedef struct Logger {
    void* impl;
    void (*log)(struct Logger* self, const char* message);
    const char* (*get_name)(struct Logger* self);
    void (*destroy)(struct Logger* self);
} Logger;

// Database 인터페이스
typedef struct Database {
    void* impl;
    char* (*save)(struct Database* self, const char* data);
    char* (*find)(struct Database* self, const char* id);
    int (*is_connected)(struct Database* self);
    void (*destroy)(struct Database* self);
} Database;
```

### 구체적 구현체
```c
// 콘솔 로거 구현
typedef struct {
    Logger base;
} ConsoleLogger;

void console_logger_log(Logger* self, const char* message) {
    printf("[CONSOLE LOG] %s\n", message);
}

Logger* create_console_logger() {
    ConsoleLogger* logger = malloc(sizeof(ConsoleLogger));
    logger->base.log = console_logger_log;
    logger->base.get_name = console_logger_get_name;
    logger->base.destroy = console_logger_destroy;
    return (Logger*)logger;
}

// 파일 로거 구현
typedef struct {
    Logger base;
    char filename[MAX_FILENAME_LEN];
} FileLogger;

Logger* create_file_logger(const char* filename) {
    FileLogger* logger = malloc(sizeof(FileLogger));
    strncpy(logger->filename, filename, MAX_FILENAME_LEN - 1);
    logger->base.log = file_logger_log;
    logger->base.get_name = file_logger_get_name;
    logger->base.destroy = file_logger_destroy;
    return (Logger*)logger;
}
```

### 의존성을 주입받는 클라이언트
```c
// UserService - 의존성을 주입받음
typedef struct UserService {
    Logger* logger;
    Database* database;
    EmailService* email_service;
} UserService;

UserService* create_user_service(Logger* logger, Database* database, EmailService* email_service) {
    if (!logger || !database || !email_service) {
        return NULL;
    }

    UserService* service = malloc(sizeof(UserService));
    service->logger = logger;
    service->database = database;
    service->email_service = email_service;
    return service;
}

char* user_service_create_user(UserService* service, const char* name, const char* email) {
    // 주입된 logger 사용
    service->logger->log(service->logger, "Creating user");

    // 주입된 database 사용
    char user_data[512];
    snprintf(user_data, 512, "{\"name\":\"%s\",\"email\":\"%s\"}", name, email);
    char* user_id = service->database->save(service->database, user_data);

    // 주입된 email service 사용
    service->email_service->send_email(service->email_service, email, "Welcome!", "Hello!");

    return user_id;
}
```

### DI 컨테이너
```c
// 간단한 DI 컨테이너
typedef struct {
    char* name;
    void* service;
    void (*destructor)(void*);
} ServiceEntry;

typedef struct DIContainer {
    ServiceEntry services[MAX_SERVICES];
    int count;
} DIContainer;

int di_container_register(DIContainer* container, const char* name, void* service, void (*destructor)(void*)) {
    ServiceEntry* entry = &container->services[container->count];
    entry->name = malloc(strlen(name) + 1);
    strcpy(entry->name, name);
    entry->service = service;
    entry->destructor = destructor;
    container->count++;
    return 1;
}

void* di_container_resolve(DIContainer* container, const char* name) {
    for (int i = 0; i < container->count; i++) {
        if (strcmp(container->services[i].name, name) == 0) {
            return container->services[i].service;
        }
    }
    return NULL;
}
```

## 사용 예제

### 1. 수동 의존성 주입
```c
// 의존성 객체들 생성
Logger* logger = create_console_logger();
Database* database = create_memory_database(10);
EmailService* email_service = create_mock_email_service();

// UserService에 의존성 주입
UserService* service = create_user_service(logger, database, email_service);

// 사용
user_service_create_user(service, "Alice", "alice@example.com");
```

### 2. 팩토리 함수를 통한 설정별 DI
```c
// 개발 환경 설정
UserService* create_development_user_service() {
    Logger* logger = create_console_logger();
    Database* database = create_memory_database(100);
    EmailService* email_service = create_mock_email_service();
    return create_user_service(logger, database, email_service);
}

// 프로덕션 환경 설정
UserService* create_production_user_service() {
    Logger* logger = create_file_logger("production.log");
    Database* database = create_memory_database(1000);
    EmailService* email_service = create_smtp_email_service("smtp.company.com");
    return create_user_service(logger, database, email_service);
}
```

### 3. DI 컨테이너 사용
```c
DIContainer* container = create_di_container();

// 서비스 등록
di_container_register(container, "logger", create_console_logger(), console_logger_destroy);
di_container_register(container, "database", create_memory_database(50), memory_db_destroy);
di_container_register(container, "email", create_smtp_email_service("smtp.example.com"), smtp_email_destroy);

// 서비스 해결
Logger* logger = (Logger*)di_container_resolve(container, "logger");
Database* database = (Database*)di_container_resolve(container, "database");
EmailService* email = (EmailService*)di_container_resolve(container, "email");

UserService* service = create_user_service(logger, database, email);
```

## 장점과 단점

### 장점
- **결합도 감소**: 객체 간의 의존성을 느슨하게 만듦
- **테스트 용이성**: Mock 객체를 쉽게 주입하여 테스트 가능
- **유연성**: 런타임에 다른 구현체로 교체 가능
- **재사용성**: 동일한 서비스를 다양한 구성으로 사용 가능
- **관심사 분리**: 객체 생성과 비즈니스 로직 분리

### 단점
- **복잡성 증가**: 의존성 관리를 위한 추가 코드 필요
- **런타임 오류**: 컴파일 시점에 의존성 누락을 발견하기 어려움
- **메모리 관리**: C언어에서는 메모리 해제 순서 관리가 복잡
- **디버깅 어려움**: 의존성 주입 체인이 복잡해질 수 있음

## 적용 상황
- **유닛 테스트**: Mock 객체를 주입하여 격리된 테스트
- **설정별 구현**: 개발/테스트/프로덕션 환경별 다른 구현체 사용
- **플러그인 시스템**: 런타임에 다른 구현체로 교체
- **서비스 지향 아키텍처**: 서비스 간의 의존성 관리
- **프레임워크 개발**: 사용자가 구현체를 제공할 수 있도록 설계

Dependency Injection 패턴은 C 언어에서 함수 포인터를 활용하여 객체 간의 결합도를 낮추고 코드의 유연성과 테스트 가능성을 크게 향상시킵니다.