#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// Thread-safe Singleton using modern C++
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

// Alternative: Meyer's Singleton (simpler, C++11 thread-safe)
class Logger {
private:
    Logger() = default;

public:
    static Logger& getInstance() {
        static Logger instance; // Thread-safe in C++11 and later
        return instance;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& message) {
        std::cout << "[LOG] " << message << std::endl;
    }
};

void workerThread(int id) {
    auto& db = DatabaseConnection::getInstance();
    db.executeQuery("SELECT * FROM users WHERE id = " + std::to_string(id));

    auto& logger = Logger::getInstance();
    logger.log("Worker thread " + std::to_string(id) + " completed");
}

int main() {
    std::cout << "=== Singleton Pattern Demo ===" << std::endl;

    // Test DatabaseConnection singleton
    auto& db1 = DatabaseConnection::getInstance();
    auto& db2 = DatabaseConnection::getInstance();

    std::cout << "Are both instances the same? " << (&db1 == &db2 ? "Yes" : "No") << std::endl;

    db1.executeQuery("SELECT * FROM products");
    db2.setConnectionString("database://remote:5432");
    db1.executeQuery("SELECT * FROM orders");

    // Test thread safety
    std::cout << "\nTesting thread safety:" << std::endl;
    std::vector<std::thread> threads;

    for (int i = 1; i <= 5; ++i) {
        threads.emplace_back(workerThread, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    // Test Logger singleton
    std::cout << "\nTesting Logger singleton:" << std::endl;
    auto& logger1 = Logger::getInstance();
    auto& logger2 = Logger::getInstance();

    std::cout << "Are both logger instances the same? " << (&logger1 == &logger2 ? "Yes" : "No") << std::endl;

    logger1.log("Application started");
    logger2.log("Application running");

    return 0;
}