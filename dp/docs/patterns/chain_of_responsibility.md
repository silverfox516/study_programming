# Chain of Responsibility Pattern

## 개요
Chain of Responsibility 패턴은 요청을 처리할 수 있는 기회를 하나 이상의 객체에게 부여함으로써 요청하는 객체와 처리하는 객체 사이의 결합도를 낮추는 행위 패턴입니다. 요청을 받으면 각 핸들러는 요청을 처리할지 아니면 체인의 다음 핸들러로 넘길지를 결정합니다.

## 구조
- **Handler**: 요청을 처리하기 위한 인터페이스를 정의하고, 후속자에 대한 링크를 구현
- **Concrete Handler (ConsoleLogger, FileLogger, EmailLogger)**: 요청을 처리하며, 후속자에 접근할 수 있음
- **Client**: 체인 상의 첫 번째 핸들러에게 요청을 전달

## C 언어 구현

### 요청 구조체 정의
```c
// 요청 레벨 정의
typedef enum {
    REQUEST_INFO,
    REQUEST_DEBUG,
    REQUEST_WARNING,
    REQUEST_ERROR,
    REQUEST_CRITICAL
} RequestLevel;

// 로그 요청 구조체
typedef struct {
    RequestLevel level;
    char* message;
    char* source;
} LogRequest;
```

### 핸들러 인터페이스
```c
// 핸들러 기본 구조체
typedef struct Handler {
    RequestLevel handled_level;
    struct Handler* next_handler;
    void (*handle_request)(struct Handler* self, LogRequest* request);
    void (*set_next)(struct Handler* self, struct Handler* next);
    void (*destroy)(struct Handler* self);
} Handler;

// 다음 핸들러 설정
void handler_set_next(Handler* self, Handler* next) {
    self->next_handler = next;
}
```

### 구체적 핸들러 구현

#### 콘솔 로거
```c
typedef struct {
    Handler base;
    char* name;
} ConsoleLogger;

void console_handle_request(Handler* self, LogRequest* request) {
    ConsoleLogger* logger = (ConsoleLogger*)self;

    // 자신의 레벨 이상이면 처리
    if (request->level >= self->handled_level) {
        printf("[CONSOLE] %s: [%s] %s (from %s)\n",
               logger->name, level_to_string(request->level),
               request->message, request->source);
    }

    // 체인의 다음 핸들러로 전달
    if (self->next_handler) {
        self->next_handler->handle_request(self->next_handler, request);
    }
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
```

#### 파일 로거
```c
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

        // 실제 구현에서는 파일에 기록
        // FILE* file = fopen(logger->filename, "a");
        // fprintf(file, "[%s] %s: %s\n", level_to_string(request->level),
        //         request->source, request->message);
        // fclose(file);
    }

    // 다음 핸들러로 전달
    if (self->next_handler) {
        self->next_handler->handle_request(self->next_handler, request);
    }
}
```

#### 이메일 로거 (중요한 오류용)
```c
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

        // 실제 구현에서는 이메일 발송
        printf("  Subject: Critical Error Alert\n");
        printf("  Body: A critical error occurred in %s: %s\n",
               request->source, request->message);
    }

    // 다음 핸들러로 전달
    if (self->next_handler) {
        self->next_handler->handle_request(self->next_handler, request);
    }
}
```

## 사용 예제

### 로깅 시스템 체인
```c
int main() {
    // 핸들러 생성
    Handler* console = console_logger_create(REQUEST_INFO, "ConsoleLogger");
    Handler* file = file_logger_create(REQUEST_WARNING, "error.log", "FileLogger");
    Handler* email = email_logger_create(REQUEST_CRITICAL, "admin@company.com", "EmailLogger");

    // 체인 구성: Console -> File -> Email
    console->set_next(console, file);
    file->set_next(file, email);

    // 다양한 레벨의 로그 요청 테스트
    LogRequest* info_req = log_request_create(REQUEST_INFO, "Application started", "Main");
    LogRequest* warning_req = log_request_create(REQUEST_WARNING, "Low disk space", "FileSystem");
    LogRequest* critical_req = log_request_create(REQUEST_CRITICAL, "Server crashed", "System");

    // 요청 처리
    console->handle_request(console, info_req);      // 콘솔만
    console->handle_request(console, warning_req);   // 콘솔 + 파일
    console->handle_request(console, critical_req);  // 콘솔 + 파일 + 이메일

    return 0;
}
```

### 고객 지원 시스템 체인
```c
// 지원 핸들러 - 우선순위별 처리
typedef struct {
    Handler base;
    char* department;
    int max_priority;
} SupportHandler;

void support_handle_request(Handler* self, LogRequest* request) {
    SupportHandler* handler = (SupportHandler*)self;
    int priority = (int)request->level;

    if (priority <= handler->max_priority) {
        printf("[SUPPORT] %s department: Handling ticket priority %d\n",
               handler->department, priority);
        printf("  Issue: %s (from %s)\n", request->message, request->source);
        return; // 처리 완료, 더 이상 전달하지 않음
    }

    // 처리할 수 없으면 다음 핸들러로 에스컬레이션
    if (self->next_handler) {
        printf("[SUPPORT] %s department: Cannot handle priority %d, escalating...\n",
               handler->department, priority);
        self->next_handler->handle_request(self->next_handler, request);
    }
}

// 사용 예제
Handler* level1 = support_handler_create(1, "Level1");
Handler* level2 = support_handler_create(3, "Level2");
Handler* manager = support_handler_create(4, "Manager");

level1->set_next(level1, level2);
level2->set_next(level2, manager);

// 요청 처리
level1->handle_request(level1, simple_req);   // Level1에서 처리
level1->handle_request(level1, complex_req);  // Level2로 에스컬레이션
level1->handle_request(level1, urgent_req);   // Manager까지 에스컬레이션
```

출력 예제:
```
[CONSOLE] ConsoleLogger: [INFO] Application started (from Main)

[CONSOLE] ConsoleLogger: [WARNING] Low disk space (from FileSystem)
[FILE] FileLogger: Writing to 'error.log': [WARNING] Low disk space (from FileSystem)

[CONSOLE] ConsoleLogger: [CRITICAL] Server crashed (from System)
[FILE] FileLogger: Writing to 'error.log': [CRITICAL] Server crashed (from System)
[EMAIL] EmailLogger: Sending alert to 'admin@company.com': [CRITICAL] Server crashed (from System)
```

## 장점과 단점

### 장점
- **결합도 감소**: 요청자와 처리자 사이의 결합도를 낮춤
- **유연성**: 런타임에 체인을 동적으로 구성할 수 있음
- **책임 분산**: 여러 객체가 요청을 처리할 기회를 가짐
- **단일 책임 원칙**: 각 핸들러는 특정 타입의 요청만 처리
- **개방/폐쇄 원칙**: 새로운 핸들러를 쉽게 추가할 수 있음

### 단점
- **성능**: 체인을 따라 요청이 전달되므로 성능 오버헤드 발생
- **디버깅 어려움**: 요청 처리 경로를 추적하기 어려울 수 있음
- **처리 보장 없음**: 체인의 어떤 핸들러도 요청을 처리하지 않을 수 있음
- **순서 의존성**: 핸들러의 순서가 결과에 영향을 미칠 수 있음

## 적용 상황
- **로깅 시스템**: 다양한 레벨의 로그를 여러 목적지로 기록
- **에러 처리**: 다양한 타입의 예외를 순차적으로 처리
- **GUI 이벤트 처리**: UI 컴포넌트에서 이벤트를 부모로 전파
- **인증 및 권한 검증**: 여러 단계의 보안 검사
- **요청 필터링**: 웹 애플리케이션에서 요청 전처리
- **파싱**: 다양한 형식의 데이터를 순차적으로 파싱 시도
- **고객 지원 시스템**: 난이도에 따른 티켓 에스컬레이션

Chain of Responsibility 패턴은 C 언어에서 함수 포인터와 연결 리스트를 활용하여 요청 처리의 유연성을 제공하는 강력한 패턴입니다.