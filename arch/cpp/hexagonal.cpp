// Hexagonal Architecture (Ports & Adapters)
// Core domain has no dependencies on external systems.
// Ports (interfaces) define what the domain needs; Adapters implement them.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// ============================================================
// PORTS (interfaces the domain defines)
// ============================================================
namespace ports {

struct Order {
    int id;
    std::string product;
    double amount;
    bool paid = false;
};

// Driven port: persistence
class OrderRepository {
public:
    virtual ~OrderRepository() = default;
    virtual void save(const Order& order) = 0;
    virtual Order* find_by_id(int id) = 0;
};

// Driven port: payment
class PaymentGateway {
public:
    virtual ~PaymentGateway() = default;
    virtual bool charge(const std::string& product, double amount) = 0;
};

// Driven port: notification
class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void notify(const std::string& message) = 0;
};

} // namespace ports

// ============================================================
// CORE DOMAIN (depends only on ports, not adapters)
// ============================================================
namespace domain {

class OrderService {
    ports::OrderRepository& repo_;
    ports::PaymentGateway& payment_;
    ports::Notifier& notifier_;
    int next_id_ = 1;

public:
    OrderService(ports::OrderRepository& repo,
                 ports::PaymentGateway& pay,
                 ports::Notifier& notify)
        : repo_(repo), payment_(pay), notifier_(notify) {}

    int place_order(const std::string& product, double amount) {
        // Domain logic — pure business rules
        if (amount <= 0)
            throw std::runtime_error("Amount must be positive");

        if (!payment_.charge(product, amount))
            throw std::runtime_error("Payment failed");

        ports::Order order{next_id_++, product, amount, true};
        repo_.save(order);
        notifier_.notify("Order #" + std::to_string(order.id) + " placed: " + product);
        return order.id;
    }
};

} // namespace domain

// ============================================================
// ADAPTERS (implement ports for specific technologies)
// ============================================================
namespace adapters {

// In-memory adapter for repository
class InMemoryOrderRepo : public ports::OrderRepository {
    std::vector<ports::Order> orders_;
public:
    void save(const ports::Order& order) override {
        orders_.push_back(order);
        std::cout << "  [InMemoryRepo] Saved order #" << order.id << "\n";
    }

    ports::Order* find_by_id(int id) override {
        for (auto& o : orders_)
            if (o.id == id) return &o;
        return nullptr;
    }
};

// Stripe adapter for payment
class StripePayment : public ports::PaymentGateway {
public:
    bool charge(const std::string& product, double amount) override {
        std::cout << "  [Stripe] Charging $" << amount << " for " << product << "\n";
        return true; // simulate success
    }
};

// Console adapter for notification
class ConsoleNotifier : public ports::Notifier {
public:
    void notify(const std::string& message) override {
        std::cout << "  [Console] " << message << "\n";
    }
};

// Slack adapter for notification (alternative)
class SlackNotifier : public ports::Notifier {
public:
    void notify(const std::string& message) override {
        std::cout << "  [Slack] #orders: " << message << "\n";
    }
};

} // namespace adapters

int main() {
    std::cout << "=== Hexagonal: Console Notifier ===\n";
    {
        adapters::InMemoryOrderRepo repo;
        adapters::StripePayment payment;
        adapters::ConsoleNotifier notifier;
        domain::OrderService service(repo, payment, notifier);

        service.place_order("Widget", 29.99);
        service.place_order("Gadget", 49.99);
    }

    std::cout << "\n=== Hexagonal: Slack Notifier (swap adapter) ===\n";
    {
        adapters::InMemoryOrderRepo repo;
        adapters::StripePayment payment;
        adapters::SlackNotifier notifier; // swapped!
        domain::OrderService service(repo, payment, notifier);

        service.place_order("Laptop", 999.99);
    }

    // Key insight: domain::OrderService never changed.
    // Only adapters were swapped.

    return 0;
}
