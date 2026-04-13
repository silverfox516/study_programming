// Single Responsibility Principle (SRP)
// A class should have only one reason to change.

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// ============================================================
// BAD: User class has multiple responsibilities
// - user data management
// - persistence (save/load)
// - formatting (toString)
// ============================================================
namespace bad {

class User {
    std::string name_;
    std::string email_;
public:
    User(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    // Responsibility 1: data access
    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

    // Responsibility 2: persistence — shouldn't be here
    void save_to_db() const {
        std::cout << "  [BAD] INSERT INTO users ('" << name_ << "', '" << email_ << "')\n";
    }

    // Responsibility 3: formatting — shouldn't be here
    std::string to_json() const {
        return "{\"name\":\"" + name_ + "\",\"email\":\"" + email_ + "\"}";
    }
};

} // namespace bad

// ============================================================
// GOOD: Separate responsibilities into focused classes
// ============================================================
namespace good {

// Responsibility 1: domain data only
class User {
    std::string name_;
    std::string email_;
public:
    User(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }
};

// Responsibility 2: persistence
class UserRepository {
public:
    void save(const User& user) const {
        std::cout << "  [GOOD] INSERT INTO users ('" << user.name()
                  << "', '" << user.email() << "')\n";
    }
};

// Responsibility 3: formatting
class UserFormatter {
public:
    static std::string to_json(const User& user) {
        return "{\"name\":\"" + user.name() + "\",\"email\":\"" + user.email() + "\"}";
    }

    static std::string to_csv(const User& user) {
        return user.name() + "," + user.email();
    }
};

} // namespace good

int main() {
    std::cout << "=== BAD: Multiple responsibilities ===\n";
    {
        bad::User u("Alice", "alice@example.com");
        u.save_to_db();
        std::cout << "  JSON: " << u.to_json() << "\n";
        // If DB schema changes, User class changes
        // If JSON format changes, User class changes
        // If validation rules change, User class changes
    }

    std::cout << "\n=== GOOD: Single responsibility each ===\n";
    {
        good::User u("Alice", "alice@example.com");
        good::UserRepository repo;
        repo.save(u);
        std::cout << "  JSON: " << good::UserFormatter::to_json(u) << "\n";
        std::cout << "  CSV:  " << good::UserFormatter::to_csv(u) << "\n";
        // DB changes → only UserRepository changes
        // Format changes → only UserFormatter changes
        // Domain rules → only User changes
    }

    return 0;
}
