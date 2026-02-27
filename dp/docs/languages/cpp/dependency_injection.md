# Dependency Injection Pattern - C++ Implementation

## 개요

Dependency Injection(DI) 패턴은 객체가 필요로 하는 의존성을 외부에서 주입받는 디자인 패턴입니다. 이 패턴은 객체 간의 결합도를 낮추고 테스트 가능성을 높이며, 코드의 유연성과 재사용성을 향상시킵니다. 특히 SOLID 원칙 중 의존성 역전 원칙(Dependency Inversion Principle)을 구현하는 핵심 패턴입니다.

## 구조

Dependency Injection의 주요 구성 요소:
- **Service**: 의존성으로 주입될 객체의 인터페이스
- **ConcreteService**: 서비스의 구체적인 구현
- **Client**: 서비스에 의존하는 클래스
- **Injector/Container**: 의존성을 생성하고 주입하는 컨테이너

## C++ 구현

### 1. 기본 서비스 인터페이스

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

// Service interfaces
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& message) = 0;
    virtual std::string getName() const = 0;
};

class Database {
public:
    virtual ~Database() = default;
    virtual std::string save(const std::string& data) = 0;
    virtual std::string find(const std::string& id) = 0;
    virtual bool isConnected() const = 0;
};

class EmailService {
public:
    virtual ~EmailService() = default;
    virtual bool sendEmail(const std::string& to, const std::string& subject, const std::string& body) = 0;
    virtual std::string getProviderName() const = 0;
};
```

### 2. 구체적인 서비스 구현

```cpp
class ConsoleLogger : public Logger {
public:
    void log(const std::string& message) override {
        std::cout << "[CONSOLE LOG] " << message << std::endl;
    }

    std::string getName() const override {
        return "ConsoleLogger";
    }
};

class FileLogger : public Logger {
private:
    std::string filename_;

public:
    explicit FileLogger(const std::string& filename) : filename_(filename) {}

    void log(const std::string& message) override {
        std::cout << "[FILE LOG to " << filename_ << "] " << message << std::endl;
    }

    std::string getName() const override {
        return "FileLogger(" + filename_ + ")";
    }
};

class InMemoryDatabase : public Database {
private:
    std::unordered_map<std::string, std::string> data_;
    static int nextId_;

public:
    std::string save(const std::string& data) override {
        std::string id = "ID" + std::to_string(++nextId_);
        data_[id] = data;
        return id;
    }

    std::string find(const std::string& id) override {
        auto it = data_.find(id);
        return (it != data_.end()) ? it->second : "";
    }

    bool isConnected() const override {
        return true;
    }
};

class MockEmailService : public EmailService {
public:
    bool sendEmail(const std::string& to, const std::string& subject, const std::string& body) override {
        std::cout << "[MOCK EMAIL] To: " << to << ", Subject: " << subject << ", Body: " << body << std::endl;
        return true;
    }

    std::string getProviderName() const override {
        return "MockEmailService";
    }
};
```

### 3. 의존성을 받는 비즈니스 서비스

```cpp
class UserService {
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Database> database_;
    std::shared_ptr<EmailService> emailService_;

public:
    // Constructor injection
    UserService(std::shared_ptr<Logger> logger,
                std::shared_ptr<Database> database,
                std::shared_ptr<EmailService> emailService)
        : logger_(std::move(logger)), database_(std::move(database)), emailService_(std::move(emailService)) {}

    std::string createUser(const std::string& name, const std::string& email) {
        logger_->log("Creating user: " + name);

        if (!database_->isConnected()) {
            logger_->log("Database connection failed");
            return "";
        }

        std::string userData = R"({"name":")" + name + R"(","email":")" + email + R"("})";
        std::string userId = database_->save(userData);

        logger_->log("User created with ID: " + userId);

        // Send welcome email
        std::string subject = "Welcome!";
        std::string body = "Hello " + name + ", welcome to our service!";
        emailService_->sendEmail(email, subject, body);

        return userId;
    }

    std::string getUser(const std::string& id) {
        logger_->log("Retrieving user with ID: " + id);
        return database_->find(id);
    }

    void printServiceInfo() {
        std::cout << "UserService Configuration:" << std::endl;
        std::cout << "  Logger: " << logger_->getName() << std::endl;
        std::cout << "  Database: Connected=" << database_->isConnected() << std::endl;
        std::cout << "  Email: " << emailService_->getProviderName() << std::endl;
    }
};
```

### 4. DI 컨테이너

```cpp
#include <any>

class DIContainer {
private:
    std::unordered_map<std::string, std::any> services_;
    std::unordered_map<std::string, std::function<std::any()>> factories_;

public:
    // Register singleton instance
    template<typename T>
    void registerInstance(const std::string& name, std::shared_ptr<T> instance) {
        services_[name] = instance;
    }

    // Register factory function
    template<typename T>
    void registerFactory(const std::string& name, std::function<std::shared_ptr<T>()> factory) {
        factories_[name] = [factory]() -> std::any {
            return factory();
        };
    }

    // Resolve service by name
    template<typename T>
    std::shared_ptr<T> resolve(const std::string& name) {
        // Try singleton instances first
        auto serviceIt = services_.find(name);
        if (serviceIt != services_.end()) {
            try {
                return std::any_cast<std::shared_ptr<T>>(serviceIt->second);
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Type mismatch for service: " << name << std::endl;
                return nullptr;
            }
        }

        // Try factories
        auto factoryIt = factories_.find(name);
        if (factoryIt != factories_.end()) {
            try {
                std::any result = factoryIt->second();
                return std::any_cast<std::shared_ptr<T>>(result);
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Factory type mismatch for service: " << name << std::endl;
                return nullptr;
            }
        }

        std::cerr << "Service not found: " << name << std::endl;
        return nullptr;
    }

    bool hasService(const std::string& name) const {
        return services_.find(name) != services_.end() ||
               factories_.find(name) != factories_.end();
    }
};
```

### 5. Builder 패턴과 함께 사용

```cpp
class UserServiceBuilder {
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Database> database_;
    std::shared_ptr<EmailService> emailService_;

public:
    UserServiceBuilder& withLogger(std::shared_ptr<Logger> logger) {
        logger_ = std::move(logger);
        return *this;
    }

    UserServiceBuilder& withDatabase(std::shared_ptr<Database> database) {
        database_ = std::move(database);
        return *this;
    }

    UserServiceBuilder& withEmailService(std::shared_ptr<EmailService> emailService) {
        emailService_ = std::move(emailService);
        return *this;
    }

    std::unique_ptr<UserService> build() {
        if (!logger_ || !database_ || !emailService_) {
            throw std::runtime_error("All dependencies must be provided");
        }
        return std::make_unique<UserService>(logger_, database_, emailService_);
    }
};
```

### 6. 환경 기반 설정

```cpp
enum class Environment {
    DEVELOPMENT,
    PRODUCTION,
    TEST
};

class ApplicationContext {
private:
    Environment environment_;
    DIContainer container_;

public:
    explicit ApplicationContext(Environment env) : environment_(env) {
        setupServices();
    }

    void setupServices() {
        switch (environment_) {
            case Environment::DEVELOPMENT:
                container_.registerInstance<Logger>("logger", std::make_shared<ConsoleLogger>());
                container_.registerInstance<Database>("database", std::make_shared<InMemoryDatabase>());
                container_.registerInstance<EmailService>("email", std::make_shared<MockEmailService>());
                break;

            case Environment::PRODUCTION:
                container_.registerFactory<Logger>("logger", []() {
                    return std::make_shared<FileLogger>("production.log");
                });
                container_.registerFactory<Database>("database", []() {
                    return std::make_shared<PostgreSQLDatabase>("postgresql://prod:5432/app");
                });
                container_.registerFactory<EmailService>("email", []() {
                    return std::make_shared<SMTPEmailService>("smtp.company.com");
                });
                break;

            case Environment::TEST:
                container_.registerInstance<Logger>("logger", std::make_shared<FileLogger>("test.log"));
                container_.registerInstance<Database>("database", std::make_shared<InMemoryDatabase>());
                container_.registerInstance<EmailService>("email", std::make_shared<MockEmailService>());
                break;
        }
    }

    std::unique_ptr<UserService> getUserService() {
        auto logger = container_.resolve<Logger>("logger");
        auto database = container_.resolve<Database>("database");
        auto emailService = container_.resolve<EmailService>("email");

        if (!logger || !database || !emailService) {
            throw std::runtime_error("Failed to resolve all dependencies");
        }

        return std::make_unique<UserService>(logger, database, emailService);
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. 수동 의존성 주입
    auto logger = std::make_shared<ConsoleLogger>();
    auto database = std::make_shared<InMemoryDatabase>();
    auto emailService = std::make_shared<MockEmailService>();

    auto userService = std::make_unique<UserService>(logger, database, emailService);
    userService->createUser("Alice Johnson", "alice@example.com");

    // 2. Builder 패턴 사용
    auto userService2 = UserServiceBuilder()
        .withLogger(std::make_shared<FileLogger>("users.log"))
        .withDatabase(std::make_shared<InMemoryDatabase>())
        .withEmailService(std::make_shared<MockEmailService>())
        .build();

    // 3. DI 컨테이너 사용
    DIContainer container;
    container.registerInstance<Logger>("logger", std::make_shared<ConsoleLogger>());
    container.registerInstance<Database>("database", std::make_shared<InMemoryDatabase>());
    container.registerFactory<EmailService>("email", []() {
        return std::make_shared<SMTPEmailService>("smtp.example.com");
    });

    auto resolvedLogger = container.resolve<Logger>("logger");
    auto resolvedDB = container.resolve<Database>("database");
    auto resolvedEmail = container.resolve<EmailService>("email");

    auto containerService = std::make_unique<UserService>(resolvedLogger, resolvedDB, resolvedEmail);

    // 4. 환경 기반 설정
    ApplicationContext devContext(Environment::DEVELOPMENT);
    auto devService = devContext.getUserService();

    ApplicationContext prodContext(Environment::PRODUCTION);
    auto prodService = prodContext.getUserService();

    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: `shared_ptr`로 자동 메모리 관리
2. **템플릿**: 타입 안전한 제네릭 컨테이너 구현
3. **std::any**: 동적 타입 저장을 통한 유연한 서비스 등록
4. **람다 함수**: 팩토리 함수의 간편한 구현
5. **RAII**: 자동 리소스 관리

## 적용 상황

1. **웹 애플리케이션**: 컨트롤러, 서비스, 레포지토리 계층
2. **테스트**: Mock 객체를 이용한 단위 테스트
3. **설정 관리**: 환경별 다른 구현 제공
4. **플러그인 시스템**: 런타임에 구현 교체
5. **마이크로서비스**: 서비스 간 의존성 관리

Dependency Injection 패턴은 SOLID 원칙을 준수하는 확장 가능하고 테스트 가능한 애플리케이션을 구축하는 데 필수적인 패턴입니다.