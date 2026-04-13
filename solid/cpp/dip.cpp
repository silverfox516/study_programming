// Dependency Inversion Principle (DIP)
// High-level modules should not depend on low-level modules.
// Both should depend on abstractions.

#include <iostream>
#include <string>
#include <memory>
#include <vector>

// ============================================================
// BAD: High-level OrderService directly depends on low-level MySqlDatabase
// ============================================================
namespace bad {

class MySqlDatabase {
public:
    void insert(const std::string& table, const std::string& data) {
        std::cout << "  [MySQL] INSERT INTO " << table << ": " << data << "\n";
    }
};

class EmailSender {
public:
    void send(const std::string& to, const std::string& msg) {
        std::cout << "  [SMTP] To: " << to << " — " << msg << "\n";
    }
};

// HIGH-LEVEL module directly depends on LOW-LEVEL modules
class OrderService {
    MySqlDatabase db_;       // concrete dependency
    EmailSender email_;      // concrete dependency
public:
    void place_order(const std::string& item) {
        db_.insert("orders", item);
        email_.send("admin@shop.com", "New order: " + item);
        std::cout << "  Order placed: " << item << "\n";
    }
    // Cannot test without a real MySQL connection
    // Cannot switch to PostgreSQL without modifying this class
};

} // namespace bad

// ============================================================
// GOOD: Both high and low level depend on abstractions
// ============================================================
namespace good {

// Abstractions (interfaces)
class Database {
public:
    virtual ~Database() = default;
    virtual void insert(const std::string& table, const std::string& data) = 0;
};

class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void notify(const std::string& to, const std::string& msg) = 0;
};

// Low-level implementations
class MySqlDatabase : public Database {
public:
    void insert(const std::string& table, const std::string& data) override {
        std::cout << "  [MySQL] INSERT INTO " << table << ": " << data << "\n";
    }
};

class PostgresDatabase : public Database {
public:
    void insert(const std::string& table, const std::string& data) override {
        std::cout << "  [Postgres] INSERT INTO " << table << ": " << data << "\n";
    }
};

class EmailNotifier : public Notifier {
public:
    void notify(const std::string& to, const std::string& msg) override {
        std::cout << "  [Email] To: " << to << " — " << msg << "\n";
    }
};

class SlackNotifier : public Notifier {
public:
    void notify(const std::string& to, const std::string& msg) override {
        std::cout << "  [Slack] #" << to << ": " << msg << "\n";
    }
};

// HIGH-LEVEL module depends on ABSTRACTIONS
class OrderService {
    std::unique_ptr<Database> db_;
    std::unique_ptr<Notifier> notifier_;
public:
    OrderService(std::unique_ptr<Database> db, std::unique_ptr<Notifier> notifier)
        : db_(std::move(db)), notifier_(std::move(notifier)) {}

    void place_order(const std::string& item) {
        db_->insert("orders", item);
        notifier_->notify("admin", "New order: " + item);
        std::cout << "  Order placed: " << item << "\n";
    }
};

} // namespace good

int main() {
    std::cout << "=== BAD: Direct dependency on concrete classes ===\n";
    {
        bad::OrderService svc;
        svc.place_order("Widget");
    }

    std::cout << "\n=== GOOD: Depends on abstractions ===\n";
    {
        // Easy to swap implementations
        std::cout << "-- With MySQL + Email:\n";
        auto svc1 = good::OrderService(
            std::make_unique<good::MySqlDatabase>(),
            std::make_unique<good::EmailNotifier>()
        );
        svc1.place_order("Widget");

        std::cout << "-- With Postgres + Slack:\n";
        auto svc2 = good::OrderService(
            std::make_unique<good::PostgresDatabase>(),
            std::make_unique<good::SlackNotifier>()
        );
        svc2.place_order("Gadget");
    }

    return 0;
}
