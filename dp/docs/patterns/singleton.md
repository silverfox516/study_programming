# Singleton Pattern (싱글톤 패턴)

## 📋 개요

Singleton 패턴은 클래스의 인스턴스가 오직 하나만 존재하도록 보장하는 생성 패턴입니다. 전역적으로 접근 가능한 단일 인스턴스를 제공하며, 해당 클래스의 객체가 시스템에서 유일함을 보장합니다.

## 🎯 목적

- 클래스의 인스턴스가 단 하나만 존재하도록 보장
- 전역 접근점 제공
- 리소스의 중복 생성 방지
- 상태를 전역적으로 공유

## 🔍 문제 상황

다음과 같은 상황에서 Singleton 패턴이 필요합니다:

1. **데이터베이스 연결**: 애플리케이션에서 하나의 DB 연결만 필요한 경우
2. **로거**: 전체 시스템에서 하나의 로깅 인스턴스만 필요한 경우
3. **설정 관리**: 애플리케이션 설정을 전역적으로 관리해야 하는 경우
4. **캐시**: 메모리 캐시를 전역적으로 관리해야 하는 경우

## 🏗️ 구조

```
┌─────────────────┐
│    Singleton    │
├─────────────────┤
│ - instance      │
├─────────────────┤
│ + getInstance() │
│ - constructor() │
└─────────────────┘
```

## 💻 언어별 구현

### C 구현

**특징:**
- 함수 포인터를 사용한 메서드 구현
- 전역 정적 변수로 인스턴스 관리
- 명시적 메모리 관리 필요

```c
// 전역 정적 변수로 인스턴스 보관
static Logger* instance = NULL;

Logger* get_logger_instance() {
    if (instance == NULL) {
        instance = malloc(sizeof(Logger));
        // 초기화 로직
    }
    return instance;
}
```

**장점:**
- 메모리 사용량 최소화
- 빠른 실행 속도

**단점:**
- 스레드 안전성 수동 구현 필요
- 메모리 누수 위험

### C++ 구현

**특징:**
- Meyer's Singleton (C++11 이후 스레드 안전)
- RAII 원칙 활용
- 스마트 포인터 사용

```cpp
class DatabaseConnection {
public:
    static DatabaseConnection& getInstance() {
        static DatabaseConnection instance;  // Thread-safe in C++11+
        return instance;
    }

private:
    DatabaseConnection() = default;
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
};
```

**장점:**
- 자동 스레드 안전성 (C++11+)
- RAII로 자동 메모리 관리
- 복사/이동 방지 가능

**단점:**
- 컴파일러 의존적

### Rust 구현

**특징:**
- `LazyLock`을 사용한 지연 초기화
- `Arc<Mutex<T>>`로 스레드 안전성 보장
- 소유권 시스템 활용

```rust
use std::sync::{Arc, Mutex, LazyLock};

static LOGGER: LazyLock<Arc<Mutex<Logger>>> = LazyLock::new(|| {
    Arc::new(Mutex::new(Logger::new()))
});

impl GlobalLogger {
    pub fn instance() -> Arc<Mutex<Logger>> {
        LOGGER.clone()
    }
}
```

**장점:**
- 컴파일 타임 스레드 안전성 보장
- 메모리 안전성
- 제로 코스트 추상화

**단점:**
- 러닝 커브 존재

## 🎨 실용적인 예제

### 1. 데이터베이스 연결 관리

```rust
// Rust 예제
pub struct DatabaseConnection {
    connection_string: String,
    is_connected: bool,
}

impl DatabaseConnection {
    pub fn execute_query(&self, query: &str) -> Result<String, String> {
        if self.is_connected {
            Ok(format!("Query result for: {}", query))
        } else {
            Err("Not connected to database".to_string())
        }
    }
}
```

### 2. 애플리케이션 설정 관리

```cpp
// C++ 예제
class AppConfig {
    std::unordered_map<std::string, std::string> settings;

public:
    static AppConfig& getInstance() {
        static AppConfig instance;
        return instance;
    }

    void setSetting(const std::string& key, const std::string& value) {
        settings[key] = value;
    }

    std::string getSetting(const std::string& key) const {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : "";
    }
};
```

### 3. 로깅 시스템

```c
// C 예제
typedef struct Logger {
    char* log_file;
    FILE* file_handle;
} Logger;

void log_message(const char* message) {
    Logger* logger = get_logger_instance();
    if (logger && logger->file_handle) {
        fprintf(logger->file_handle, "[LOG] %s\n", message);
        fflush(logger->file_handle);
    }
}
```

## ⚖️ 장단점

### 장점

1. **유일한 인스턴스 보장**: 시스템에서 하나의 인스턴스만 존재
2. **전역 접근점**: 어디서든 쉽게 접근 가능
3. **지연 초기화**: 실제 필요할 때까지 생성 지연 가능
4. **메모리 절약**: 불필요한 객체 생성 방지

### 단점

1. **테스트 어려움**: 전역 상태로 인한 테스트 격리 문제
2. **의존성 은닉**: 클래스 간 의존관계가 명확하지 않음
3. **확장성 제한**: 나중에 여러 인스턴스가 필요해질 수 있음
4. **스레드 안전성**: 멀티스레드 환경에서 추가 고려 필요

## 🔄 관련 패턴

- **Factory Pattern**: Singleton으로 팩토리 인스턴스 관리
- **Facade Pattern**: 복잡한 서브시스템의 단일 접근점
- **State Pattern**: 상태 관리자를 Singleton으로 구현

## ⚠️ 안티패턴 주의사항

### 1. 전역 변수 남용
```cpp
// 나쁜 예
class GlobalData {
    static GlobalData instance;
public:
    int data1, data2, data3; // 모든 것을 여기에 저장
};
```

### 2. 테스트 불가능한 코드
```cpp
// 나쁜 예 - 테스트하기 어려움
class Service {
    void doSomething() {
        Logger::getInstance().log("Something happened");
        // 테스트에서 로거를 모킹하기 어려움
    }
};
```

### 3. 숨겨진 의존성
```cpp
// 나쁜 예
class Calculator {
    int calculate(int a, int b) {
        // 숨겨진 의존성
        Config& config = Config::getInstance();
        return a + b + config.getOffset();
    }
};
```

## 🛠️ 모범 사례

### 1. 의존성 주입과 결합
```cpp
// 좋은 예
class Service {
    Logger& logger;
public:
    Service(Logger& logger = Logger::getInstance())
        : logger(logger) {}

    void doSomething() {
        logger.log("Something happened");
    }
};
```

### 2. 인터페이스 사용
```cpp
// 좋은 예
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

class FileLogger : public ILogger {
public:
    static ILogger& getInstance() {
        static FileLogger instance;
        return instance;
    }
};
```

### 3. 스레드 안전한 구현
```rust
// Rust - 자동으로 스레드 안전
use std::sync::{Arc, Mutex, LazyLock};

static INSTANCE: LazyLock<Arc<Mutex<MyService>>> = LazyLock::new(|| {
    Arc::new(Mutex::new(MyService::new()))
});
```

## 🧪 테스트 전략

### 1. 인터페이스 기반 테스트
```cpp
void testWithMockLogger() {
    MockLogger mockLogger;
    Service service(mockLogger);
    service.doSomething();
    ASSERT_TRUE(mockLogger.wasLogCalled());
}
```

### 2. Singleton 리셋 기능
```cpp
class TestableService {
public:
    static void resetForTesting() {
        // 테스트용 리셋 기능
        instance.reset();
    }
};
```

## 📊 성능 고려사항

### 메모리 사용량
- **C**: 최소 메모리 사용
- **C++**: RAII로 안전한 메모리 관리
- **Rust**: 컴파일 타임 최적화

### 스레드 성능
- **C**: 수동 동기화 필요
- **C++**: 표준 보장된 스레드 안전성
- **Rust**: 제로 코스트 동기화

## 🔚 결론

Singleton 패턴은 강력하지만 신중하게 사용해야 하는 패턴입니다. 전역 상태의 필요성을 먼저 검토하고, 가능하다면 의존성 주입과 같은 대안을 고려하는 것이 좋습니다.

**사용하기 좋은 경우:**
- 로깅, 설정 관리, 캐시 등
- 리소스가 제한적이고 공유가 필요한 경우

**피해야 할 경우:**
- 단순히 전역 변수를 대체하려는 경우
- 테스트 가능성이 중요한 경우
- 미래에 확장 가능성이 있는 경우