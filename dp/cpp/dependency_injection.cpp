#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeinfo>
#include <any>

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

// Concrete implementations
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

int InMemoryDatabase::nextId_ = 0;

class PostgreSQLDatabase : public Database {
private:
    std::string connectionString_;
    bool connected_;

public:
    explicit PostgreSQLDatabase(const std::string& connectionString)
        : connectionString_(connectionString), connected_(true) {
        std::cout << "Connected to PostgreSQL: " << connectionString << std::endl;
    }

    std::string save(const std::string& data) override {
        if (!connected_) return "";
        std::cout << "Saving to PostgreSQL: " << data << std::endl;
        return "POSTGRES_ID_" + std::to_string(std::hash<std::string>{}(data) % 1000);
    }

    std::string find(const std::string& id) override {
        if (!connected_) return "";
        return "PostgreSQL data for " + id;
    }

    bool isConnected() const override {
        return connected_;
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

class SMTPEmailService : public EmailService {
private:
    std::string smtpServer_;

public:
    explicit SMTPEmailService(const std::string& smtpServer) : smtpServer_(smtpServer) {}

    bool sendEmail(const std::string& to, const std::string& subject, const std::string& body) override {
        std::cout << "[SMTP via " << smtpServer_ << "] To: " << to << ", Subject: " << subject << ", Body: " << body << std::endl;
        return true;
    }

    std::string getProviderName() const override {
        return "SMTP(" + smtpServer_ + ")";
    }
};

// Business service that depends on other services
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

// Simple Dependency Injection Container
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

    // Check if service exists
    bool hasService(const std::string& name) const {
        return services_.find(name) != services_.end() ||
               factories_.find(name) != factories_.end();
    }

    // List all registered services
    std::vector<std::string> getServiceNames() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : services_) {
            names.push_back(name + " (instance)");
        }
        for (const auto& [name, _] : factories_) {
            names.push_back(name + " (factory)");
        }
        return names;
    }
};

// Builder pattern for UserService
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

// Factory for creating configured services
class ServiceFactory {
public:
    static std::unique_ptr<UserService> createDevelopmentUserService() {
        auto logger = std::make_shared<ConsoleLogger>();
        auto database = std::make_shared<InMemoryDatabase>();
        auto emailService = std::make_shared<MockEmailService>();

        return std::make_unique<UserService>(logger, database, emailService);
    }

    static std::unique_ptr<UserService> createProductionUserService() {
        auto logger = std::make_shared<FileLogger>("production.log");
        auto database = std::make_shared<PostgreSQLDatabase>("postgresql://prod-server:5432/users");
        auto emailService = std::make_shared<SMTPEmailService>("smtp.company.com");

        return std::make_unique<UserService>(logger, database, emailService);
    }

    static std::unique_ptr<UserService> createTestUserService() {
        auto logger = std::make_shared<FileLogger>("test.log");
        auto database = std::make_shared<InMemoryDatabase>();
        auto emailService = std::make_shared<MockEmailService>();

        return std::make_unique<UserService>(logger, database, emailService);
    }
};

// Configuration-based service creation
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

    DIContainer& getContainer() {
        return container_;
    }
};

int main() {
    std::cout << "=== Dependency Injection Pattern Demo ===\n\n";

    try {
        // Method 1: Manual dependency injection
        std::cout << "1. Manual Dependency Injection:\n";
        auto logger = std::make_shared<ConsoleLogger>();
        auto database = std::make_shared<InMemoryDatabase>();
        auto emailService = std::make_shared<MockEmailService>();

        auto userService = std::make_unique<UserService>(logger, database, emailService);
        userService->printServiceInfo();

        std::string userId = userService->createUser("Alice Johnson", "alice@example.com");
        std::cout << "Created user with ID: " << userId << std::endl;

        std::string userData = userService->getUser(userId);
        std::cout << "Retrieved user data: " << userData << "\n\n";

        // Method 2: Using builder pattern
        std::cout << "2. Builder Pattern for DI:\n";
        auto userService2 = UserServiceBuilder()
            .withLogger(std::make_shared<FileLogger>("users.log"))
            .withDatabase(std::make_shared<InMemoryDatabase>())
            .withEmailService(std::make_shared<MockEmailService>())
            .build();

        userService2->printServiceInfo();
        userService2->createUser("Bob Smith", "bob@example.com");

        std::cout << "\n";

        // Method 3: Using factory
        std::cout << "3. Factory Pattern for DI:\n";
        auto devService = ServiceFactory::createDevelopmentUserService();
        std::cout << "Development Service:\n";
        devService->printServiceInfo();
        devService->createUser("Charlie Brown", "charlie@example.com");

        std::cout << "\nProduction Service:\n";
        auto prodService = ServiceFactory::createProductionUserService();
        prodService->printServiceInfo();
        prodService->createUser("Diana Prince", "diana@example.com");

        std::cout << "\n";

        // Method 4: DI Container
        std::cout << "4. DI Container:\n";
        DIContainer container;

        // Register services
        container.registerInstance<Logger>("console_logger", std::make_shared<ConsoleLogger>());
        container.registerInstance<Logger>("file_logger", std::make_shared<FileLogger>("app.log"));
        container.registerInstance<Database>("memory_db", std::make_shared<InMemoryDatabase>());
        container.registerFactory<EmailService>("smtp_email", []() {
            return std::make_shared<SMTPEmailService>("smtp.example.com");
        });

        // List all services
        std::cout << "Registered services:\n";
        for (const auto& name : container.getServiceNames()) {
            std::cout << "  - " << name << std::endl;
        }

        // Resolve and use services
        auto resolvedLogger = container.resolve<Logger>("console_logger");
        auto resolvedDB = container.resolve<Database>("memory_db");
        auto resolvedEmail = container.resolve<EmailService>("smtp_email");

        if (resolvedLogger && resolvedDB && resolvedEmail) {
            auto containerService = std::make_unique<UserService>(resolvedLogger, resolvedDB, resolvedEmail);
            std::cout << "\nContainer-resolved service:\n";
            containerService->printServiceInfo();
            containerService->createUser("Eve Adams", "eve@example.com");
        }

        std::cout << "\n";

        // Method 5: Application Context (Environment-based)
        std::cout << "5. Environment-based Configuration:\n";

        std::vector<std::pair<Environment, std::string>> environments = {
            {Environment::DEVELOPMENT, "Development"},
            {Environment::PRODUCTION, "Production"},
            {Environment::TEST, "Test"}
        };

        for (const auto& [env, name] : environments) {
            std::cout << "\n" << name << " Environment:\n";
            ApplicationContext context(env);
            auto envService = context.getUserService();
            envService->printServiceInfo();
            envService->createUser("User " + name, "user@" + name + ".com");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n✅ Dependency Injection provides flexibility, testability,\n";
    std::cout << "   and loose coupling between components!\n";

    return 0;
}