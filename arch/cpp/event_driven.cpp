// Event-Driven Architecture
// Components communicate through events (pub-sub), not direct calls.
// Enables loose coupling and async processing.

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <any>

// ============================================================
// Event infrastructure
// ============================================================
struct Event {
    std::string type;
    std::unordered_map<std::string, std::string> data;
};

using EventHandler = std::function<void(const Event&)>;

class EventBus {
    std::unordered_map<std::string, std::vector<EventHandler>> subscribers_;
public:
    void subscribe(const std::string& event_type, EventHandler handler) {
        subscribers_[event_type].push_back(std::move(handler));
    }

    void publish(const Event& event) {
        std::cout << "[EventBus] Publishing: " << event.type << "\n";
        auto it = subscribers_.find(event.type);
        if (it != subscribers_.end())
            for (const auto& handler : it->second)
                handler(event);
    }
};

// ============================================================
// Independent services — communicate only via events
// ============================================================
class OrderService {
    EventBus& bus_;
    int next_id_ = 1;
public:
    explicit OrderService(EventBus& bus) : bus_(bus) {}

    void place_order(const std::string& customer, const std::string& product, double amount) {
        int id = next_id_++;
        std::cout << "[OrderService] Creating order #" << id << "\n";

        bus_.publish({"order.placed", {
            {"order_id", std::to_string(id)},
            {"customer", customer},
            {"product", product},
            {"amount", std::to_string(amount)},
        }});
    }
};

class PaymentService {
    EventBus& bus_;
public:
    explicit PaymentService(EventBus& bus) : bus_(bus) {
        bus_.subscribe("order.placed", [this](const Event& e) {
            handle_order_placed(e);
        });
    }

    void handle_order_placed(const Event& e) {
        auto order_id = e.data.at("order_id");
        auto amount = e.data.at("amount");
        std::cout << "  [PaymentService] Charging $" << amount << " for order #" << order_id << "\n";

        bus_.publish({"payment.completed", {
            {"order_id", order_id},
            {"status", "success"},
        }});
    }
};

class InventoryService {
    EventBus& bus_;
public:
    explicit InventoryService(EventBus& bus) : bus_(bus) {
        bus_.subscribe("order.placed", [this](const Event& e) {
            auto product = e.data.at("product");
            std::cout << "  [InventoryService] Reserving: " << product << "\n";
        });
    }
};

class NotificationService {
    EventBus& bus_;
public:
    explicit NotificationService(EventBus& bus) : bus_(bus) {
        bus_.subscribe("payment.completed", [this](const Event& e) {
            auto order_id = e.data.at("order_id");
            std::cout << "  [NotificationService] Sending confirmation for order #" << order_id << "\n";
        });

        bus_.subscribe("order.placed", [this](const Event& e) {
            auto customer = e.data.at("customer");
            std::cout << "  [NotificationService] Order received from " << customer << "\n";
        });
    }
};

class AuditService {
    EventBus& bus_;
public:
    explicit AuditService(EventBus& bus) : bus_(bus) {
        // Subscribe to ALL event types for audit
        for (const auto& type : {"order.placed", "payment.completed"}) {
            bus_.subscribe(type, [](const Event& e) {
                std::cout << "  [AuditService] Logged: " << e.type << "\n";
            });
        }
    }
};

int main() {
    EventBus bus;

    // Services register themselves — order doesn't matter
    PaymentService payment(bus);
    InventoryService inventory(bus);
    NotificationService notifications(bus);
    AuditService audit(bus);
    OrderService orders(bus);

    // Place orders — event cascade happens automatically
    std::cout << "=== Order 1 ===\n";
    orders.place_order("Alice", "Laptop", 999.99);

    std::cout << "\n=== Order 2 ===\n";
    orders.place_order("Bob", "Mouse", 29.99);

    // Key insight:
    // - OrderService doesn't know about Payment, Inventory, or Notifications
    // - Adding a new service (e.g., Analytics) requires ZERO changes to existing code
    // - Services can be deployed independently

    return 0;
}
