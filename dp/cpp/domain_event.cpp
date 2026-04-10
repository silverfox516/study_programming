// Domain Event — DDD Pattern
//
// Intent: capture something meaningful that happened in the domain as a
//         first-class object. Other parts of the system (or other bounded
//         contexts) react to these events asynchronously.
//
// When to use:
//   - Loose coupling between modules (order placed -> inventory reserved)
//   - Audit trail / event sourcing (see dp/cpp/event_sourcing.cpp)
//   - Triggering side effects without polluting the domain model
//
// When NOT to use:
//   - The "event" is really a command in disguise (then use a command bus)
//   - Eventual consistency is unacceptable for this use case

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <variant>

// ============================================================================
// Events
// ============================================================================
struct OrderPlaced {
    int         order_id;
    std::string customer;
    double      total;
    std::chrono::system_clock::time_point when = std::chrono::system_clock::now();
};

struct OrderShipped {
    int         order_id;
    std::string carrier;
    std::chrono::system_clock::time_point when = std::chrono::system_clock::now();
};

using DomainEvent = std::variant<OrderPlaced, OrderShipped>;

// ============================================================================
// Simple in-process event bus
// ============================================================================
class EventBus {
public:
    using Handler = std::function<void(const DomainEvent&)>;

    void subscribe(Handler h) { handlers_.push_back(std::move(h)); }

    void publish(const DomainEvent& event) {
        for (const auto& h : handlers_) h(event);
    }

private:
    std::vector<Handler> handlers_;
};

// ============================================================================
// Subscribers
// ============================================================================
void inventory_handler(const DomainEvent& e) {
    if (auto* op = std::get_if<OrderPlaced>(&e)) {
        std::cout << "  [inventory] reserving stock for order #" << op->order_id << "\n";
    }
}

void notification_handler(const DomainEvent& e) {
    std::visit([](const auto& ev) {
        using T = std::decay_t<decltype(ev)>;
        if constexpr (std::is_same_v<T, OrderPlaced>) {
            std::cout << "  [notify] email to " << ev.customer << ": order placed ($"
                      << ev.total << ")\n";
        } else if constexpr (std::is_same_v<T, OrderShipped>) {
            std::cout << "  [notify] email: order #" << ev.order_id
                      << " shipped via " << ev.carrier << "\n";
        }
    }, e);
}

void audit_handler(const DomainEvent& e) {
    std::visit([](const auto& ev) {
        auto secs = std::chrono::system_clock::to_time_t(ev.when);
        std::cout << "  [audit] event recorded at " << secs << "\n";
    }, e);
}

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Domain Event Pattern ===\n";
    EventBus bus;
    bus.subscribe(inventory_handler);
    bus.subscribe(notification_handler);
    bus.subscribe(audit_handler);

    std::cout << "publishing OrderPlaced:\n";
    bus.publish(OrderPlaced{1, "Alice", 99.95});

    std::cout << "\npublishing OrderShipped:\n";
    bus.publish(OrderShipped{1, "FedEx"});

    std::cout << "\nKey points:\n";
    std::cout << " * Events are past-tense facts — 'OrderPlaced', not 'PlaceOrder'\n";
    std::cout << " * Publishers don't know about subscribers — loose coupling\n";
    std::cout << " * Subscribers react independently — inventory, notifications, audit\n";
    std::cout << " * Compare with Observer (dp/cpp/observer.cpp): Observer is in-object,\n";
    std::cout << "   Domain Events cross module/context boundaries\n";
    return 0;
}
