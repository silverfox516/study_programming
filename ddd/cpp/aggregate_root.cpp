// Aggregate Root
// A cluster of domain objects treated as a single unit for data changes.
// External access only through the root; root enforces invariants.

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <numeric>

// ============================================================
// Order Aggregate — OrderLine is only accessible through Order
// ============================================================
class OrderLine {
    std::string product_;
    int quantity_;
    double unit_price_;
public:
    OrderLine(std::string product, int qty, double price)
        : product_(std::move(product)), quantity_(qty), unit_price_(price) {
        if (qty <= 0) throw std::invalid_argument("Quantity must be positive");
        if (price < 0) throw std::invalid_argument("Price cannot be negative");
    }

    const std::string& product() const { return product_; }
    int quantity() const { return quantity_; }
    double subtotal() const { return quantity_ * unit_price_; }
};

class Order {
    int id_;
    std::string customer_;
    std::vector<OrderLine> lines_;
    bool confirmed_ = false;
    double max_total_;

    // INVARIANT: total never exceeds max_total_
    void check_total() const {
        if (total() > max_total_)
            throw std::runtime_error("Order exceeds maximum allowed total");
    }

public:
    Order(int id, std::string customer, double max_total = 10000.0)
        : id_(id), customer_(std::move(customer)), max_total_(max_total) {}

    // All mutations go through the Aggregate Root
    void add_line(const std::string& product, int qty, double price) {
        if (confirmed_)
            throw std::runtime_error("Cannot modify confirmed order");

        lines_.emplace_back(product, qty, price);

        try {
            check_total(); // enforce invariant
        } catch (...) {
            lines_.pop_back(); // rollback
            throw;
        }

        std::cout << "  [Order #" << id_ << "] Added: " << product
                  << " x" << qty << " @ $" << price << "\n";
    }

    void remove_last_line() {
        if (confirmed_) throw std::runtime_error("Cannot modify confirmed order");
        if (lines_.empty()) throw std::runtime_error("No lines to remove");
        std::cout << "  [Order #" << id_ << "] Removed: " << lines_.back().product() << "\n";
        lines_.pop_back();
    }

    void confirm() {
        if (lines_.empty())
            throw std::runtime_error("Cannot confirm empty order");
        confirmed_ = true;
        std::cout << "  [Order #" << id_ << "] CONFIRMED: " << lines_.size()
                  << " items, total $" << total() << "\n";
    }

    double total() const {
        double sum = 0;
        for (const auto& line : lines_) sum += line.subtotal();
        return sum;
    }

    int id() const { return id_; }
    bool confirmed() const { return confirmed_; }
    size_t line_count() const { return lines_.size(); }
};

// ============================================================
// BAD: Direct access to internal entities breaks invariants
// ============================================================
namespace bad_example {

void demo() {
    std::cout << "=== BAD: Direct manipulation (conceptual) ===\n";
    std::cout << "  // order.lines_.push_back(...)  -- bypasses validation\n";
    std::cout << "  // order.confirmed_ = true       -- bypasses empty check\n";
    std::cout << "  // These are impossible because fields are private!\n";
}

} // namespace bad_example

int main() {
    bad_example::demo();

    std::cout << "\n=== GOOD: All access through Aggregate Root ===\n";

    Order order(1, "Alice", 500.0);
    order.add_line("Widget", 2, 29.99);
    order.add_line("Gadget", 1, 149.99);

    // Invariant enforced: total would exceed $500
    try {
        order.add_line("Expensive", 1, 400.00);
    } catch (const std::exception& e) {
        std::cout << "  Rejected: " << e.what() << "\n";
    }

    std::cout << "  Lines: " << order.line_count()
              << ", Total: $" << order.total() << "\n";

    order.confirm();

    // Cannot modify after confirmation
    try {
        order.add_line("Extra", 1, 9.99);
    } catch (const std::exception& e) {
        std::cout << "  Rejected: " << e.what() << "\n";
    }

    return 0;
}
