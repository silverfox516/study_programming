// CQRS — Command Query Responsibility Segregation
//
// Intent: separate the model that handles state-changing commands from the
//         model that answers queries. Commands return nothing (or just an
//         acknowledgement). Queries never change state.
//
// When to use:
//   - Read and write workloads have very different shapes
//   - You want to optimize reads (denormalized views) without complicating writes
//   - Pairs naturally with Event Sourcing — see dp/cpp/event_sourcing.cpp
//
// When NOT to use:
//   - Simple CRUD apps — the duplication isn't worth it
//   - You can't accept eventual consistency between command and query sides
//   - The team is small and learning costs outweigh the benefits

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <functional>
#include <memory>
#include <stdexcept>

// ============================================================================
// Domain: a tiny order management system
// ============================================================================
struct OrderCreated {
    int          order_id;
    std::string  customer;
    double       amount;
};
struct OrderCancelled { int order_id; };
struct OrderShipped   { int order_id; std::string carrier; };

using DomainEvent = std::variant<OrderCreated, OrderCancelled, OrderShipped>;

// ============================================================================
// Command side: write model. Validates and emits events.
// ============================================================================
struct CreateOrderCommand { int order_id; std::string customer; double amount; };
struct CancelOrderCommand { int order_id; };
struct ShipOrderCommand   { int order_id; std::string carrier; };

class CommandHandler {
public:
    using EventSink = std::function<void(const DomainEvent&)>;
    explicit CommandHandler(EventSink sink) : sink_(std::move(sink)) {}

    void handle(const CreateOrderCommand& c) {
        if (c.amount <= 0)            throw std::invalid_argument("amount must be positive");
        if (active_.count(c.order_id)) throw std::invalid_argument("order already exists");
        active_.insert(c.order_id);
        sink_(OrderCreated{c.order_id, c.customer, c.amount});
    }

    void handle(const CancelOrderCommand& c) {
        if (!active_.count(c.order_id)) throw std::invalid_argument("no such order");
        active_.erase(c.order_id);
        sink_(OrderCancelled{c.order_id});
    }

    void handle(const ShipOrderCommand& c) {
        if (!active_.count(c.order_id)) throw std::invalid_argument("no such order");
        sink_(OrderShipped{c.order_id, c.carrier});
    }

private:
    EventSink                sink_;
    std::unordered_set<int>  active_;   // simplified state
};

// ============================================================================
// Query side: read model. Optimized for the queries we need.
// ============================================================================
struct OrderSummary {
    int          order_id;
    std::string  customer;
    double       amount;
    std::string  status;       // "open" / "cancelled" / "shipped"
    std::string  carrier;
};

class OrderReadModel {
public:
    void apply(const OrderCreated& e) {
        rows_[e.order_id] = OrderSummary{e.order_id, e.customer, e.amount, "open", ""};
    }
    void apply(const OrderCancelled& e) {
        if (auto it = rows_.find(e.order_id); it != rows_.end()) {
            it->second.status = "cancelled";
        }
    }
    void apply(const OrderShipped& e) {
        if (auto it = rows_.find(e.order_id); it != rows_.end()) {
            it->second.status  = "shipped";
            it->second.carrier = e.carrier;
        }
    }

    // Queries — never change state
    std::vector<OrderSummary> all_open() const {
        std::vector<OrderSummary> out;
        for (const auto& [id, row] : rows_) {
            if (row.status == "open") out.push_back(row);
        }
        return out;
    }

    OrderSummary get(int id) const {
        return rows_.at(id);
    }

private:
    std::unordered_map<int, OrderSummary> rows_;
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== CQRS Pattern ===\n";
    OrderReadModel read_model;

    // Wire the command side to publish events into the read model
    auto sink = [&read_model](const DomainEvent& e) {
        std::visit([&](const auto& concrete) { read_model.apply(concrete); }, e);
    };
    CommandHandler commands(sink);

    // Issue commands
    commands.handle(CreateOrderCommand{1, "Alice", 250.0});
    commands.handle(CreateOrderCommand{2, "Bob",   75.5});
    commands.handle(CreateOrderCommand{3, "Carol", 500.0});
    commands.handle(ShipOrderCommand  {1, "FedEx"});
    commands.handle(CancelOrderCommand{2});

    // Run a query
    std::cout << "all open orders:\n";
    for (const auto& s : read_model.all_open()) {
        std::cout << "  #" << s.order_id << " " << s.customer
                  << " $" << s.amount << " (" << s.status << ")\n";
    }

    auto one = read_model.get(1);
    std::cout << "order 1: " << one.customer
              << " status=" << one.status << " carrier=" << one.carrier << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Command side validates, mutates, emits events\n";
    std::cout << " * Query side projects events into a read-optimized shape\n";
    std::cout << " * Two sides can be scaled and stored independently\n";
    std::cout << " * Pairs naturally with event_sourcing.cpp\n";
    return 0;
}
