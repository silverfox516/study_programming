# Singleton Pattern - C++ Implementation

## 개요

Singleton 패턴은 특정 클래스의 인스턴스가 오직 하나만 생성되도록 보장하고, 이에 대한 전역적인 접근점을 제공하는 생성 패턴입니다. C++에서는 스레드 안전성, 메모리 관리, 초기화 시점 등을 고려하여 여러 가지 방식으로 구현할 수 있습니다.

## 구조

Singleton 패턴의 주요 구성 요소:
- **Private Constructor**: 외부에서 직접 인스턴스 생성 방지
- **Static Instance**: 유일한 인스턴스를 저장하는 정적 멤버
- **Static Access Method**: 인스턴스에 접근하는 정적 메서드
- **Copy Prevention**: 복사 생성자와 대입 연산자 비활성화

## C++ 구현

### 1. 스레드 안전한 Singleton (std::once_flag 사용)

```cpp
class DatabaseConnection {
private:
    static std::unique_ptr<DatabaseConnection> instance;
    static std::once_flag init_flag;

    // Private constructor
    DatabaseConnection() : connection_string("database://localhost:5432") {
        std::cout << "Database connection established\n";
    }

    std::string connection_string;

public:
    // Delete copy constructor and assignment operator
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    // Thread-safe getInstance method
    static DatabaseConnection& getInstance() {
        std::call_once(init_flag, []() {
            instance = std::unique_ptr<DatabaseConnection>(new DatabaseConnection());
        });
        return *instance;
    }

    void executeQuery(const std::string& query) {
        std::cout << "Executing query: " << query << " on " << connection_string << std::endl;
    }

    void setConnectionString(const std::string& conn_str) {
        connection_string = conn_str;
    }

    const std::string& getConnectionString() const {
        return connection_string;
    }
};

// Static member definitions
std::unique_ptr<DatabaseConnection> DatabaseConnection::instance = nullptr;
std::once_flag DatabaseConnection::init_flag;
```

### 2. Meyer's Singleton (C++11+ 스레드 안전)

```cpp
class Logger {
private:
    Logger() = default;

public:
    static Logger& getInstance() {
        static Logger instance; // C++11부터 스레드 안전
        return instance;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& message) {
        std::cout << "[LOG] " << message << std::endl;
    }
};
```

## 사용 예제

### 기본 사용법
```cpp
// DatabaseConnection singleton 사용
auto& db1 = DatabaseConnection::getInstance();
auto& db2 = DatabaseConnection::getInstance();

std::cout << "Are both instances the same? " << (&db1 == &db2 ? "Yes" : "No") << std::endl;

db1.executeQuery("SELECT * FROM products");
db2.setConnectionString("database://remote:5432");
db1.executeQuery("SELECT * FROM orders");
```

### 스레드 안전성 테스트
```cpp
void workerThread(int id) {
    auto& db = DatabaseConnection::getInstance();
    db.executeQuery("SELECT * FROM users WHERE id = " + std::to_string(id));

    auto& logger = Logger::getInstance();
    logger.log("Worker thread " + std::to_string(id) + " completed");
}

int main() {
    // 여러 스레드에서 동시 접근 테스트
    std::vector<std::thread> threads;

    for (int i = 1; i <= 5; ++i) {
        threads.emplace_back(workerThread, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
```

### Logger 사용 예제
```cpp
auto& logger1 = Logger::getInstance();
auto& logger2 = Logger::getInstance();

std::cout << "Are both logger instances the same? "
          << (&logger1 == &logger2 ? "Yes" : "No") << std::endl;

logger1.log("Application started");
logger2.log("Application running");
```

## C++의 장점

### 1. 스레드 안전성

#### std::once_flag 방식
- **성능 우수**: 초기화 후에는 오버헤드 없음
- **명시적 제어**: 초기화 시점을 명확히 제어
- **예외 안전**: 초기화 실패 시 재시도 가능

#### Meyer's Singleton
- **간단함**: C++11부터 자동으로 스레드 안전
- **지연 초기화**: 첫 호출 시점에 초기화
- **컴파일러 최적화**: 정적 변수 초기화 최적화

### 2. 메모리 관리

```cpp
// 스마트 포인터를 사용한 자동 메모리 관리
static std::unique_ptr<DatabaseConnection> instance;

// RAII 패턴으로 자원 자동 해제
DatabaseConnection() : connection_string("database://localhost:5432") {
    // 리소스 초기화
}
```

### 3. 타입 안전성

```cpp
// 복사 생성자와 대입 연산자 명시적 삭제
DatabaseConnection(const DatabaseConnection&) = delete;
DatabaseConnection& operator=(const DatabaseConnection&) = delete;

// 컴파일 타임에 복사 시도를 방지
```

### 4. RAII (Resource Acquisition Is Initialization)
```cpp
class DatabaseConnection {
private:
    std::string connection_string;
    // 기타 리소스들 (파일 핸들, 네트워크 연결 등)

public:
    ~DatabaseConnection() {
        // 소멸자에서 리소스 자동 해제
        std::cout << "Database connection closed\n";
    }
};
```

### 5. 성능 최적화

#### 지연 초기화 (Lazy Initialization)
```cpp
static Logger& getInstance() {
    static Logger instance; // 첫 호출 시에만 생성
    return instance;
}
```

#### 빠른 접근
```cpp
// 참조 반환으로 포인터 역참조 오버헤드 제거
static Logger& getInstance() {
    return instance;
}
```

## 구현 방식 비교

### 1. Eager Initialization
```cpp
class EagerSingleton {
private:
    static EagerSingleton instance;
    EagerSingleton() = default;

public:
    static EagerSingleton& getInstance() {
        return instance;
    }
};

EagerSingleton EagerSingleton::instance; // 프로그램 시작 시 초기화
```

**장점**: 스레드 안전, 간단함
**단점**: 메모리 낭비 가능성, 초기화 순서 문제

### 2. Lazy Initialization (Meyer's Singleton)
```cpp
class LazySingleton {
private:
    LazySingleton() = default;

public:
    static LazySingleton& getInstance() {
        static LazySingleton instance; // 첫 호출 시 초기화
        return instance;
    }
};
```

**장점**: 메모리 효율성, C++11+ 스레드 안전
**단점**: 첫 호출 시 약간의 오버헤드

### 3. Double-Checked Locking (비추천)
```cpp
class DoubleCheckedSingleton {
private:
    static std::atomic<DoubleCheckedSingleton*> instance;
    static std::mutex mutex;

public:
    static DoubleCheckedSingleton* getInstance() {
        DoubleCheckedSingleton* tmp = instance.load();
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            tmp = instance.load();
            if (tmp == nullptr) {
                tmp = new DoubleCheckedSingleton;
                instance.store(tmp);
            }
        }
        return tmp;
    }
};
```

**문제점**: 복잡하고 버그가 발생하기 쉬움, 메모리 순서 문제

## 적용 상황

### 1. 시스템 리소스 관리
- **데이터베이스 연결**: 연결 풀 관리
- **로거**: 전역 로깅 시스템
- **설정 관리자**: 애플리케이션 설정

### 2. 캐시 시스템
```cpp
class CacheManager {
public:
    static CacheManager& getInstance() {
        static CacheManager instance;
        return instance;
    }

    void put(const std::string& key, const std::string& value) {
        cache_[key] = value;
    }

    std::optional<std::string> get(const std::string& key) {
        auto it = cache_.find(key);
        return it != cache_.end() ? std::make_optional(it->second) : std::nullopt;
    }

private:
    std::unordered_map<std::string, std::string> cache_;
};
```

### 3. 하드웨어 인터페이스
```cpp
class HardwareController {
public:
    static HardwareController& getInstance() {
        static HardwareController instance;
        return instance;
    }

    void initializeHardware() {
        // 하드웨어 초기화 로직
    }

private:
    HardwareController() {
        initializeHardware();
    }
};
```

## 주의사항

### 1. 의존성 주입과의 충돌
- 테스트 어려움
- 강한 결합 발생

### 2. 멀티스레딩 환경
- 스레드 안전한 구현 필요
- 성능 고려사항

### 3. 메모리 누수 방지
```cpp
// 스마트 포인터 사용 권장
static std::unique_ptr<Singleton> instance;
```

### 4. 초기화 순서 문제
- 정적 초기화 순서는 컴파일 단위별로 정의되지 않음
- 다른 Singleton에 의존하는 경우 문제 발생 가능

## 대안 패턴

### 1. 의존성 주입
```cpp
class Service {
public:
    Service(Logger& logger) : logger_(logger) {}

private:
    Logger& logger_;
};
```

### 2. Monostate 패턴
```cpp
class Monostate {
public:
    void setValue(int value) { value_ = value; }
    int getValue() const { return value_; }

private:
    static int value_; // 모든 인스턴스가 공유
};
```

Singleton 패턴은 강력한 도구이지만, 남용하면 코드의 테스트 가능성과 유연성을 해칠 수 있으므로 신중하게 사용해야 합니다. C++11 이후의 Meyer's Singleton이 가장 권장되는 구현 방식입니다.