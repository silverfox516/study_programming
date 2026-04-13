// Tell, Don't Ask
// Tell objects to perform actions rather than asking for data and acting on it externally.

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ============================================================
// BAD: Ask for data, decide externally (procedural style)
// ============================================================
namespace bad {

class Account {
public:
    double balance = 0;
    bool is_locked = false;
    std::string owner;

    Account(std::string o, double b) : owner(std::move(o)), balance(b) {}
};

// External code queries state and makes decisions
void transfer(Account& from, Account& to, double amount) {
    // "Asking" — pulls data out and decides here
    if (from.is_locked) {
        std::cout << "  [BAD] Account locked\n";
        return;
    }
    if (from.balance < amount) {
        std::cout << "  [BAD] Insufficient funds\n";
        return;
    }
    from.balance -= amount;
    to.balance += amount;
    std::cout << "  [BAD] Transferred $" << amount
              << " from " << from.owner << " to " << to.owner << "\n";
}

} // namespace bad

// ============================================================
// GOOD: Tell the object what to do — it manages its own state
// ============================================================
namespace good {

class Account {
    double balance_;
    bool locked_;
    std::string owner_;

public:
    Account(std::string owner, double balance)
        : balance_(balance), locked_(false), owner_(std::move(owner)) {}

    // Tell: withdraw handles its own invariants
    void withdraw(double amount) {
        if (locked_)
            throw std::runtime_error("Account " + owner_ + " is locked");
        if (amount > balance_)
            throw std::runtime_error("Insufficient funds in " + owner_);
        balance_ -= amount;
    }

    // Tell: deposit is self-contained
    void deposit(double amount) {
        if (locked_)
            throw std::runtime_error("Account " + owner_ + " is locked");
        balance_ += amount;
    }

    void lock() { locked_ = true; }
    double balance() const { return balance_; }
    const std::string& owner() const { return owner_; }
};

// Transfer just "tells" — doesn't inspect internal state
void transfer(Account& from, Account& to, double amount) {
    from.withdraw(amount);  // Tell from to withdraw
    to.deposit(amount);     // Tell to to deposit
    std::cout << "  [GOOD] Transferred $" << amount
              << " from " << from.owner() << " to " << to.owner() << "\n";
}

// Another example: order validation
class Order {
    std::vector<std::pair<std::string, double>> items_;
    bool confirmed_ = false;

public:
    void add_item(const std::string& name, double price) {
        if (confirmed_)
            throw std::runtime_error("Cannot modify confirmed order");
        items_.emplace_back(name, price);
    }

    // Tell: order computes its own total
    void confirm() {
        if (items_.empty())
            throw std::runtime_error("Cannot confirm empty order");
        confirmed_ = true;
        double total = 0;
        for (const auto& [name, price] : items_) total += price;
        std::cout << "  [GOOD] Order confirmed: " << items_.size()
                  << " items, total $" << total << "\n";
    }
};

} // namespace good

int main() {
    std::cout << "=== BAD: Ask then act ===\n";
    {
        bad::Account alice("Alice", 100);
        bad::Account bob("Bob", 50);
        bad::transfer(alice, bob, 30);
        std::cout << "  Alice: $" << alice.balance << ", Bob: $" << bob.balance << "\n";
    }

    std::cout << "\n=== GOOD: Tell, don't ask ===\n";
    {
        good::Account alice("Alice", 100);
        good::Account bob("Bob", 50);
        good::transfer(alice, bob, 30);
        std::cout << "  Alice: $" << alice.balance()
                  << ", Bob: $" << bob.balance() << "\n";

        // Order example
        good::Order order;
        order.add_item("Widget", 29.99);
        order.add_item("Gadget", 49.99);
        order.confirm();

        try {
            order.add_item("Extra", 9.99);  // fails — already confirmed
        } catch (const std::exception& e) {
            std::cout << "  Expected error: " << e.what() << "\n";
        }
    }

    return 0;
}
