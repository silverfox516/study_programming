#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>

// Customer interface
class Customer {
public:
    virtual ~Customer() = default;

    virtual void greet() const = 0;
    virtual void purchase(const std::string& item) = 0;
    virtual int getDiscountRate() const = 0;
    virtual bool isNull() const = 0;
    virtual std::string getName() const = 0;
    virtual int getId() const = 0;
};

// Real Customer implementation
class RealCustomer : public Customer {
private:
    int id_;
    std::string name_;
    std::string email_;
    int loyaltyPoints_;

public:
    RealCustomer(int id, const std::string& name, const std::string& email)
        : id_(id), name_(name), email_(email), loyaltyPoints_(0) {}

    void greet() const override {
        std::cout << "Hello " << name_ << "! Welcome back!" << std::endl;
    }

    void purchase(const std::string& item) override {
        std::cout << name_ << " purchased: " << item << std::endl;
        loyaltyPoints_ += 10;
        std::cout << "Loyalty points: " << loyaltyPoints_ << std::endl;
    }

    int getDiscountRate() const override {
        if (loyaltyPoints_ > 100) return 15;
        if (loyaltyPoints_ > 50) return 10;
        return 5;
    }

    bool isNull() const override {
        return false;
    }

    std::string getName() const override {
        return name_;
    }

    int getId() const override {
        return id_;
    }

    const std::string& getEmail() const {
        return email_;
    }

    int getLoyaltyPoints() const {
        return loyaltyPoints_;
    }
};

// Null Customer implementation (Singleton)
class NullCustomer : public Customer {
private:
    static std::unique_ptr<NullCustomer> instance_;

public:
    static NullCustomer& getInstance() {
        if (!instance_) {
            instance_ = std::unique_ptr<NullCustomer>(new NullCustomer());
        }
        return *instance_;
    }

    void greet() const override {
        std::cout << "Welcome, guest!" << std::endl;
    }

    void purchase(const std::string& item) override {
        std::cout << "Please register to purchase: " << item << std::endl;
    }

    int getDiscountRate() const override {
        return 0; // No discount for null customer
    }

    bool isNull() const override {
        return true;
    }

    std::string getName() const override {
        return "Guest";
    }

    int getId() const override {
        return -1;
    }

    // Non-copyable, non-movable
    NullCustomer(const NullCustomer&) = delete;
    NullCustomer& operator=(const NullCustomer&) = delete;
    NullCustomer(NullCustomer&&) = delete;
    NullCustomer& operator=(NullCustomer&&) = delete;

private:
    NullCustomer() = default;
};

// Initialize static member
std::unique_ptr<NullCustomer> NullCustomer::instance_ = nullptr;

// Customer Repository
class CustomerRepository {
private:
    std::unordered_map<int, std::unique_ptr<RealCustomer>> customers_;

public:
    void addCustomer(std::unique_ptr<RealCustomer> customer) {
        if (customer) {
            customers_[customer->getId()] = std::move(customer);
        }
    }

    Customer& findCustomer(int id) {
        auto it = customers_.find(id);
        if (it != customers_.end()) {
            return *(it->second);
        }
        // Return null object instead of nullptr or throwing exception
        return NullCustomer::getInstance();
    }

    size_t getCustomerCount() const {
        return customers_.size();
    }

    std::vector<std::reference_wrapper<Customer>> getAllCustomers() {
        std::vector<std::reference_wrapper<Customer>> result;
        for (const auto& [id, customer] : customers_) {
            result.emplace_back(*customer);
        }
        return result;
    }
};

// Service using customers
class CustomerService {
private:
    CustomerRepository repository_;

public:
    void addCustomer(int id, const std::string& name, const std::string& email) {
        auto customer = std::make_unique<RealCustomer>(id, name, email);
        repository_.addCustomer(std::move(customer));
    }

    void processOrder(int customerId, const std::string& item, double price) {
        Customer& customer = repository_.findCustomer(customerId);

        // No null check needed - null object handles it gracefully
        customer.greet();

        int discount = customer.getDiscountRate();
        double finalPrice = price * (100 - discount) / 100.0;

        std::cout << "Processing order for customer ID " << customerId << std::endl;
        std::cout << "Item: " << item << ", Original price: $" << price << std::endl;
        std::cout << "Discount: " << discount << "%, Final price: $" << finalPrice << std::endl;

        customer.purchase(item);

        if (customer.isNull()) {
            std::cout << "Note: This was a guest purchase" << std::endl;
        }

        std::cout << "---" << std::endl;
    }

    size_t getCustomerCount() const {
        return repository_.getCustomerCount();
    }
};

// Logger interface for another Null Object example
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& level, const std::string& message) = 0;
    virtual bool isNull() const { return false; }
    virtual std::string getName() const = 0;
};

// Console Logger
class ConsoleLogger : public Logger {
public:
    void log(const std::string& level, const std::string& message) override {
        std::cout << "[" << level << "] " << message << std::endl;
    }

    std::string getName() const override {
        return "ConsoleLogger";
    }
};

// File Logger
class FileLogger : public Logger {
private:
    std::string filename_;

public:
    explicit FileLogger(const std::string& filename) : filename_(filename) {}

    void log(const std::string& level, const std::string& message) override {
        std::cout << "[FILE LOG to " << filename_ << "] [" << level << "] " << message << std::endl;
    }

    std::string getName() const override {
        return "FileLogger(" + filename_ + ")";
    }
};

// Null Logger (Singleton)
class NullLogger : public Logger {
private:
    static std::unique_ptr<NullLogger> instance_;

public:
    static NullLogger& getInstance() {
        if (!instance_) {
            instance_ = std::unique_ptr<NullLogger>(new NullLogger());
        }
        return *instance_;
    }

    void log(const std::string& level, const std::string& message) override {
        // Do nothing - silent logging
    }

    bool isNull() const override {
        return true;
    }

    std::string getName() const override {
        return "NullLogger";
    }

    // Non-copyable, non-movable
    NullLogger(const NullLogger&) = delete;
    NullLogger& operator=(const NullLogger&) = delete;
    NullLogger(NullLogger&&) = delete;
    NullLogger& operator=(NullLogger&&) = delete;

private:
    NullLogger() = default;
};

std::unique_ptr<NullLogger> NullLogger::instance_ = nullptr;

// Application using logger
class Application {
private:
    Logger* logger_;
    std::string name_;

public:
    Application(const std::string& name, Logger* logger = nullptr)
        : name_(name), logger_(logger ? logger : &NullLogger::getInstance()) {}

    void run() {
        logger_->log("INFO", name_ + " application starting");
        logger_->log("DEBUG", "Processing data");

        // Simulate some work
        for (int i = 1; i <= 3; ++i) {
            logger_->log("DEBUG", "Processing step " + std::to_string(i));
        }

        logger_->log("INFO", name_ + " application finished");

        if (logger_->isNull()) {
            std::cout << "(Logging was disabled for " << name_ << ")" << std::endl;
        }
    }
};

// Database connection interface
class DatabaseConnection {
public:
    virtual ~DatabaseConnection() = default;
    virtual std::vector<std::string> executeQuery(const std::string& query) = 0;
    virtual bool isConnected() const = 0;
    virtual bool isNull() const { return false; }
    virtual std::string getConnectionInfo() const = 0;
};

// Real database connection
class PostgreSQLConnection : public DatabaseConnection {
private:
    std::string connectionString_;
    bool connected_;

public:
    explicit PostgreSQLConnection(const std::string& connectionString)
        : connectionString_(connectionString), connected_(true) {
        std::cout << "Connected to PostgreSQL: " << connectionString << std::endl;
    }

    std::vector<std::string> executeQuery(const std::string& query) override {
        if (!connected_) {
            return {};
        }

        std::cout << "Executing query on PostgreSQL: " << query << std::endl;
        return {"Result for: " + query};
    }

    bool isConnected() const override {
        return connected_;
    }

    std::string getConnectionInfo() const override {
        return "PostgreSQL: " + connectionString_;
    }
};

// Null database connection
class NullDatabaseConnection : public DatabaseConnection {
private:
    static std::unique_ptr<NullDatabaseConnection> instance_;

public:
    static NullDatabaseConnection& getInstance() {
        if (!instance_) {
            instance_ = std::unique_ptr<NullDatabaseConnection>(new NullDatabaseConnection());
        }
        return *instance_;
    }

    std::vector<std::string> executeQuery(const std::string& query) override {
        std::cout << "Database not available. Query ignored: " << query << std::endl;
        return {"No database connection available"};
    }

    bool isConnected() const override {
        return false;
    }

    bool isNull() const override {
        return true;
    }

    std::string getConnectionInfo() const override {
        return "No Database Connection";
    }

    // Non-copyable, non-movable
    NullDatabaseConnection(const NullDatabaseConnection&) = delete;
    NullDatabaseConnection& operator=(const NullDatabaseConnection&) = delete;
    NullDatabaseConnection(NullDatabaseConnection&&) = delete;
    NullDatabaseConnection& operator=(NullDatabaseConnection&&) = delete;

private:
    NullDatabaseConnection() = default;
};

std::unique_ptr<NullDatabaseConnection> NullDatabaseConnection::instance_ = nullptr;

// Database service
class DatabaseService {
private:
    DatabaseConnection* connection_;

public:
    explicit DatabaseService(DatabaseConnection* connection = nullptr)
        : connection_(connection ? connection : &NullDatabaseConnection::getInstance()) {}

    std::vector<std::string> fetchUsers() {
        return connection_->executeQuery("SELECT * FROM users");
    }

    bool isDatabaseAvailable() const {
        return connection_->isConnected() && !connection_->isNull();
    }

    std::string getConnectionInfo() const {
        return connection_->getConnectionInfo();
    }
};

// Factory class for creating services with or without dependencies
class ServiceFactory {
public:
    static std::unique_ptr<CustomerService> createCustomerService() {
        return std::make_unique<CustomerService>();
    }

    static std::unique_ptr<Application> createApplication(const std::string& name,
                                                        bool enableLogging = false) {
        Logger* logger = nullptr;
        if (enableLogging) {
            static ConsoleLogger consoleLogger; // Keep alive
            logger = &consoleLogger;
        }
        return std::make_unique<Application>(name, logger);
    }

    static std::unique_ptr<DatabaseService> createDatabaseService(bool useRealDB = false) {
        if (useRealDB) {
            static PostgreSQLConnection realDB("postgresql://localhost:5432/mydb"); // Keep alive
            return std::make_unique<DatabaseService>(&realDB);
        }
        return std::make_unique<DatabaseService>();
    }
};

int main() {
    std::cout << "=== Null Object Pattern Demo ===\n\n";

    // 1. Customer service example
    std::cout << "1. Customer Service Example:\n";
    auto service = ServiceFactory::createCustomerService();

    // Add real customers
    service->addCustomer(1, "Alice Johnson", "alice@example.com");
    service->addCustomer(2, "Bob Smith", "bob@example.com");

    std::cout << "Total customers: " << service->getCustomerCount() << "\n\n";

    // Process orders for existing customers
    service->processOrder(1, "Laptop", 1000.0);
    service->processOrder(2, "Mouse", 50.0);

    // Process order for non-existent customer - null object handles it gracefully
    service->processOrder(999, "Keyboard", 100.0);

    // 2. Logger example
    std::cout << "\n2. Logger Example:\n";

    std::cout << "With console logger:\n";
    auto appWithLogger = ServiceFactory::createApplication("MyApp", true);
    appWithLogger->run();

    std::cout << "\nWith null logger (silent):\n";
    auto appWithoutLogger = ServiceFactory::createApplication("SilentApp", false);
    appWithoutLogger->run();

    // 3. Database example
    std::cout << "\n3. Database Example:\n";

    // Service with real database
    auto dbServiceWithDB = ServiceFactory::createDatabaseService(true);
    std::cout << "Database info: " << dbServiceWithDB->getConnectionInfo() << std::endl;
    std::cout << "Database available: " << std::boolalpha << dbServiceWithDB->isDatabaseAvailable() << std::endl;

    auto users = dbServiceWithDB->fetchUsers();
    std::cout << "Users fetched: " << users.size() << std::endl;
    for (const auto& user : users) {
        std::cout << "  - " << user << std::endl;
    }

    std::cout << "\nOffline service:\n";
    auto offlineService = ServiceFactory::createDatabaseService(false);
    std::cout << "Database info: " << offlineService->getConnectionInfo() << std::endl;
    std::cout << "Database available: " << std::boolalpha << offlineService->isDatabaseAvailable() << std::endl;

    auto offlineUsers = offlineService->fetchUsers();
    std::cout << "Users fetched: " << offlineUsers.size() << std::endl;
    for (const auto& user : offlineUsers) {
        std::cout << "  - " << user << std::endl;
    }

    // 4. Demonstrate polymorphism with null objects
    std::cout << "\n4. Polymorphic Null Object Usage:\n";

    std::vector<std::unique_ptr<Logger>> loggers;
    loggers.push_back(std::make_unique<ConsoleLogger>());
    loggers.push_back(std::make_unique<FileLogger>("app.log"));
    // Note: We can't put singleton null logger in vector, but we can reference it

    std::vector<Logger*> loggerRefs;
    loggerRefs.push_back(loggers[0].get());
    loggerRefs.push_back(loggers[1].get());
    loggerRefs.push_back(&NullLogger::getInstance());

    for (size_t i = 0; i < loggerRefs.size(); ++i) {
        std::cout << "Logger " << (i + 1) << " (" << loggerRefs[i]->getName() << "):\n";
        loggerRefs[i]->log("INFO", "This is a test message");
        if (loggerRefs[i]->isNull()) {
            std::cout << "  ^ This logger is null (silent)\n";
        }
    }

    std::cout << "\n✅ Null Object pattern prevents null pointer errors\n";
    std::cout << "   and provides graceful default behavior!\n";

    return 0;
}