// Bounded Context
// Each context has its own model of the same real-world concept.
// Contexts communicate through well-defined interfaces (Anti-Corruption Layer, events).

#include <iostream>
#include <string>
#include <vector>

// ============================================================
// Sales Context — "Customer" means buyer with orders
// ============================================================
namespace sales {

struct Customer {
    int id;
    std::string name;
    std::string shipping_address;
    double credit_limit;
};

class OrderService {
public:
    void place_order(const Customer& c, const std::string& product, double amount) {
        if (amount > c.credit_limit) {
            std::cout << "  [Sales] Order rejected: exceeds credit limit\n";
            return;
        }
        std::cout << "  [Sales] Order placed for " << c.name
                  << ": " << product << " ($" << amount << ")\n";
    }
};

} // namespace sales

// ============================================================
// Support Context — "Customer" means ticket submitter
// ============================================================
namespace support {

struct Customer {
    int id;
    std::string name;
    std::string email;
    std::string priority_level; // "gold", "silver", "bronze"
};

class TicketService {
public:
    void create_ticket(const Customer& c, const std::string& issue) {
        std::cout << "  [Support] Ticket from " << c.name
                  << " (" << c.priority_level << "): " << issue << "\n";
    }
};

} // namespace support

// ============================================================
// Billing Context — "Customer" means account with payment info
// ============================================================
namespace billing {

struct Customer {
    int id;
    std::string name;
    std::string payment_method;
    double outstanding_balance;
};

class InvoiceService {
public:
    void generate_invoice(const Customer& c, double amount) {
        std::cout << "  [Billing] Invoice for " << c.name
                  << ": $" << amount << " via " << c.payment_method << "\n";
    }
};

} // namespace billing

// ============================================================
// Context Map — how contexts relate
// ============================================================
namespace integration {

// Shared Kernel: common customer ID
struct CustomerRef {
    int id;
    std::string name;
};

// Translate between contexts
sales::Customer to_sales(const CustomerRef& ref) {
    return {ref.id, ref.name, "123 Main St", 5000.0};
}

support::Customer to_support(const CustomerRef& ref) {
    return {ref.id, ref.name, ref.name + "@example.com", "gold"};
}

billing::Customer to_billing(const CustomerRef& ref) {
    return {ref.id, ref.name, "credit_card", 0.0};
}

} // namespace integration

int main() {
    // Same real-world customer, different representations per context
    integration::CustomerRef alice{1, "Alice"};

    std::cout << "=== Same customer across Bounded Contexts ===\n\n";

    std::cout << "Sales Context:\n";
    sales::OrderService orders;
    orders.place_order(integration::to_sales(alice), "Laptop", 999.99);

    std::cout << "\nSupport Context:\n";
    support::TicketService tickets;
    tickets.create_ticket(integration::to_support(alice), "Screen flickering");

    std::cout << "\nBilling Context:\n";
    billing::InvoiceService invoices;
    invoices.generate_invoice(integration::to_billing(alice), 999.99);

    // Key insight:
    // - Each context has its own Customer type with different fields
    // - No shared "God object" Customer that all contexts depend on
    // - Contexts are free to evolve independently

    return 0;
}
