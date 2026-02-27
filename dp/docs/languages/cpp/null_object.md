# Null Object Pattern - C++ Implementation

## 개요

Null Object 패턴은 null 참조를 대신하여 기본 동작을 수행하는 객체를 제공하는 행위 패턴입니다. null 검사의 필요성을 제거하고 코드를 더 안전하고 간결하게 만듭니다.

## 구조

- **AbstractObject**: 실제 객체와 null 객체의 공통 인터페이스
- **RealObject**: 실제 비즈니스 로직을 구현하는 객체
- **NullObject**: 기본 동작이나 빈 동작을 제공하는 객체

## C++ 구현

```cpp
#include <iostream>
#include <memory>
#include <string>

// Abstract Logger interface
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& message) = 0;
    virtual void error(const std::string& message) = 0;
    virtual void debug(const std::string& message) = 0;
    virtual bool isEnabled() const = 0;
};

// Real Logger implementation
class FileLogger : public Logger {
private:
    std::string filename;

public:
    explicit FileLogger(const std::string& filename) : filename(filename) {}

    void log(const std::string& message) override {
        std::cout << "[FILE LOG to " << filename << "] " << message << std::endl;
    }

    void error(const std::string& message) override {
        std::cout << "[FILE ERROR to " << filename << "] " << message << std::endl;
    }

    void debug(const std::string& message) override {
        std::cout << "[FILE DEBUG to " << filename << "] " << message << std::endl;
    }

    bool isEnabled() const override {
        return true;
    }
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << "[CONSOLE] " << message << std::endl;
    }

    void error(const std::string& message) override {
        std::cout << "[CONSOLE ERROR] " << message << std::endl;
    }

    void debug(const std::string& message) override {
        std::cout << "[CONSOLE DEBUG] " << message << std::endl;
    }

    bool isEnabled() const override {
        return true;
    }
};

// Null Object implementation
class NullLogger : public Logger {
public:
    void log(const std::string& message) override {
        // Do nothing
    }

    void error(const std::string& message) override {
        // Do nothing
    }

    void debug(const std::string& message) override {
        // Do nothing
    }

    bool isEnabled() const override {
        return false;
    }
};

// Service class that uses Logger
class DatabaseService {
private:
    std::shared_ptr<Logger> logger;

public:
    explicit DatabaseService(std::shared_ptr<Logger> logger = nullptr)
        : logger(logger ? logger : std::make_shared<NullLogger>()) {}

    void setLogger(std::shared_ptr<Logger> newLogger) {
        logger = newLogger ? newLogger : std::make_shared<NullLogger>();
    }

    void connect() {
        logger->log("Connecting to database...");
        // Database connection logic
        logger->log("Connected successfully");
    }

    void disconnect() {
        logger->log("Disconnecting from database...");
        // Database disconnection logic
        logger->log("Disconnected");
    }

    void executeQuery(const std::string& query) {
        logger->debug("Executing query: " + query);

        // Simulate query execution
        if (query.empty()) {
            logger->error("Empty query provided");
            return;
        }

        logger->log("Query executed successfully");
    }
};

// Another example: Customer management
class Customer {
public:
    virtual ~Customer() = default;
    virtual std::string getName() const = 0;
    virtual std::string getEmail() const = 0;
    virtual void sendNotification(const std::string& message) = 0;
    virtual bool isVip() const = 0;
};

class RealCustomer : public Customer {
private:
    std::string name;
    std::string email;
    bool vipStatus;

public:
    RealCustomer(const std::string& name, const std::string& email, bool vipStatus = false)
        : name(name), email(email), vipStatus(vipStatus) {}

    std::string getName() const override {
        return name;
    }

    std::string getEmail() const override {
        return email;
    }

    void sendNotification(const std::string& message) override {
        std::cout << "Sending notification to " << name << " (" << email << "): " << message << std::endl;
    }

    bool isVip() const override {
        return vipStatus;
    }
};

class NullCustomer : public Customer {
public:
    std::string getName() const override {
        return "Guest";
    }

    std::string getEmail() const override {
        return "";
    }

    void sendNotification(const std::string& message) override {
        // Do nothing - guest customers don't receive notifications
    }

    bool isVip() const override {
        return false;
    }
};

class CustomerService {
private:
    std::shared_ptr<Customer> currentCustomer;

public:
    CustomerService() : currentCustomer(std::make_shared<NullCustomer>()) {}

    void setCustomer(std::shared_ptr<Customer> customer) {
        currentCustomer = customer ? customer : std::make_shared<NullCustomer>();
    }

    void processOrder() {
        std::cout << "Processing order for: " << currentCustomer->getName() << std::endl;

        if (currentCustomer->isVip()) {
            std::cout << "Applying VIP discount" << std::endl;
        }

        currentCustomer->sendNotification("Your order has been processed");
    }
};

// Factory for creating loggers
class LoggerFactory {
public:
    enum class LoggerType {
        FILE,
        CONSOLE,
        NULL_LOGGER
    };

    static std::shared_ptr<Logger> createLogger(LoggerType type, const std::string& config = "") {
        switch (type) {
            case LoggerType::FILE:
                return std::make_shared<FileLogger>(config.empty() ? "default.log" : config);
            case LoggerType::CONSOLE:
                return std::make_shared<ConsoleLogger>();
            case LoggerType::NULL_LOGGER:
            default:
                return std::make_shared<NullLogger>();
        }
    }
};
```

## 사용 예제

```cpp
int main() {
    std::cout << "=== Null Object Pattern Demo ===" << std::endl;

    // 1. Logger example
    std::cout << "\n1. Database Service with different loggers:" << std::endl;

    // Service with file logger
    auto fileLogger = LoggerFactory::createLogger(LoggerFactory::LoggerType::FILE, "database.log");
    DatabaseService dbService1(fileLogger);

    dbService1.connect();
    dbService1.executeQuery("SELECT * FROM users");
    dbService1.disconnect();

    std::cout << "\n--- Switching to console logger ---" << std::endl;

    // Switch to console logger
    auto consoleLogger = LoggerFactory::createLogger(LoggerFactory::LoggerType::CONSOLE);
    dbService1.setLogger(consoleLogger);

    dbService1.connect();
    dbService1.executeQuery("");  // This will trigger an error
    dbService1.disconnect();

    std::cout << "\n--- Switching to null logger ---" << std::endl;

    // Switch to null logger (no output)
    auto nullLogger = LoggerFactory::createLogger(LoggerFactory::LoggerType::NULL_LOGGER);
    dbService1.setLogger(nullLogger);

    dbService1.connect();  // No output
    dbService1.executeQuery("SELECT * FROM products");  // No output
    dbService1.disconnect();  // No output

    // 2. Customer example
    std::cout << "\n2. Customer Service example:" << std::endl;

    CustomerService customerService;

    // Process order for guest (null customer)
    std::cout << "\nProcessing order for guest:" << std::endl;
    customerService.processOrder();

    // Set real customer
    auto customer = std::make_shared<RealCustomer>("John Doe", "john@example.com", true);
    customerService.setCustomer(customer);

    std::cout << "\nProcessing order for registered customer:" << std::endl;
    customerService.processOrder();

    // Set null customer explicitly
    customerService.setCustomer(nullptr);  // Will default to NullCustomer

    std::cout << "\nProcessing order after setting null customer:" << std::endl;
    customerService.processOrder();

    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: 안전한 null 처리와 메모리 관리
2. **공유 소유권**: `shared_ptr`로 객체 공유
3. **팩토리 패턴**: 적절한 null 객체 생성
4. **RAII**: 자동 리소스 관리

## Null Object vs nullptr

### 전통적인 방식 (null 검사)
```cpp
void processData(Logger* logger) {
    if (logger) {
        logger->log("Processing data...");
    }
    // ... processing logic
    if (logger) {
        logger->log("Data processed");
    }
}
```

### Null Object 패턴
```cpp
void processData(Logger& logger) {
    logger.log("Processing data...");
    // ... processing logic
    logger.log("Data processed");
}
```

## 적용 상황

1. **로깅 시스템**: 선택적 로깅 구현
2. **이벤트 핸들러**: 기본 또는 빈 이벤트 처리
3. **설정 객체**: 기본 설정값 제공
4. **콜백 함수**: 기본 콜백 동작
5. **상태 객체**: 기본 상태 동작

Null Object 패턴은 null 검사 코드를 제거하고 예외 상황을 우아하게 처리할 수 있게 해주는 유용한 패턴입니다.