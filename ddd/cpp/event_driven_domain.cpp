// Event-Driven Domain
// Domain objects emit events when state changes.
// Other parts of the system react to events, maintaining loose coupling.

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>

// ============================================================
// Domain Events
// ============================================================
struct DomainEvent {
    virtual ~DomainEvent() = default;
    virtual std::string type() const = 0;
};

struct OrderCreated : DomainEvent {
    int order_id;
    std::string customer;
    double total;
    std::string type() const override { return "OrderCreated"; }
};

struct OrderItemAdded : DomainEvent {
    int order_id;
    std::string product;
    int quantity;
    std::string type() const override { return "OrderItemAdded"; }
};

struct OrderConfirmed : DomainEvent {
    int order_id;
    double final_total;
    std::string type() const override { return "OrderConfirmed"; }
};

// ============================================================
// Aggregate that emits events
// ============================================================
class Order {
    int id_;
    std::string customer_;
    double total_ = 0;
    bool confirmed_ = false;

    // Collected events — published after transaction completes
    std::vector<std::shared_ptr<DomainEvent>> pending_events_;

    void raise(std::shared_ptr<DomainEvent> event) {
        pending_events_.push_back(std::move(event));
    }

public:
    Order(int id, std::string customer) : id_(id), customer_(std::move(customer)) {
        auto evt = std::make_shared<OrderCreated>();
        evt->order_id = id_;
        evt->customer = customer_;
        evt->total = 0;
        raise(evt);
    }

    void add_item(const std::string& product, int qty, double price) {
        if (confirmed_) return;
        total_ += qty * price;

        auto evt = std::make_shared<OrderItemAdded>();
        evt->order_id = id_;
        evt->product = product;
        evt->quantity = qty;
        raise(evt);
    }

    void confirm() {
        if (confirmed_) return;
        confirmed_ = true;

        auto evt = std::make_shared<OrderConfirmed>();
        evt->order_id = id_;
        evt->final_total = total_;
        raise(evt);
    }

    // Return and clear pending events
    std::vector<std::shared_ptr<DomainEvent>> collect_events() {
        auto events = std::move(pending_events_);
        pending_events_.clear();
        return events;
    }
};

// ============================================================
// Event Dispatcher + Handlers
// ============================================================
using Handler = std::function<void(const DomainEvent&)>;

class EventDispatcher {
    std::vector<Handler> handlers_;
public:
    void subscribe(Handler h) { handlers_.push_back(std::move(h)); }

    void dispatch(const std::vector<std::shared_ptr<DomainEvent>>& events) {
        for (const auto& event : events)
            for (const auto& handler : handlers_)
                handler(*event);
    }
};

// Handlers (separate bounded contexts / services)
void inventory_handler(const DomainEvent& event) {
    if (auto* e = dynamic_cast<const OrderItemAdded*>(&event)) {
        std::cout << "  [Inventory] Reserve " << e->quantity << "x " << e->product << "\n";
    }
}

void billing_handler(const DomainEvent& event) {
    if (auto* e = dynamic_cast<const OrderConfirmed*>(&event)) {
        std::cout << "  [Billing] Generate invoice for order #" << e->order_id
                  << ": $" << e->final_total << "\n";
    }
}

void notification_handler(const DomainEvent& event) {
    if (auto* e = dynamic_cast<const OrderCreated*>(&event)) {
        std::cout << "  [Notification] Welcome, " << e->customer << "!\n";
    }
    if (auto* e = dynamic_cast<const OrderConfirmed*>(&event)) {
        std::cout << "  [Notification] Order #" << e->order_id << " confirmed!\n";
    }
}

void audit_handler(const DomainEvent& event) {
    std::cout << "  [Audit] " << event.type() << "\n";
}

int main() {
    // Setup
    EventDispatcher dispatcher;
    dispatcher.subscribe(inventory_handler);
    dispatcher.subscribe(billing_handler);
    dispatcher.subscribe(notification_handler);
    dispatcher.subscribe(audit_handler);

    // Domain operations
    std::cout << "=== Creating Order ===\n";
    Order order(1, "Alice");
    dispatcher.dispatch(order.collect_events());

    std::cout << "\n=== Adding Items ===\n";
    order.add_item("Widget", 2, 29.99);
    order.add_item("Gadget", 1, 49.99);
    dispatcher.dispatch(order.collect_events());

    std::cout << "\n=== Confirming Order ===\n";
    order.confirm();
    dispatcher.dispatch(order.collect_events());

    return 0;
}
