// Table Module — Enterprise Pattern (PoEAA)
//
// Intent: one class per database table, containing all the business logic that
//         operates on that table's rows. The module works with a "record set"
//         (an in-memory table) rather than individual domain objects.
//
// When to use:
//   - Business logic is simple and maps naturally to table operations
//   - You have a record-set / dataset abstraction (ADO.NET, JDBC ResultSet style)
//   - Quick prototyping before committing to a richer domain model
//
// When NOT to use:
//   - Complex business rules that span multiple tables — Domain Model is better
//   - You need polymorphism, rich behavior per row — use entities

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

// ============================================================================
// RecordSet — simulates a "table in memory" (rows × columns)
// ============================================================================
struct Row {
    int         id;
    std::string name;
    double      balance;
    bool        active;
};

class RecordSet {
public:
    void add_row(Row r) { rows_.push_back(std::move(r)); }
    std::vector<Row>& rows() { return rows_; }
    const std::vector<Row>& rows() const { return rows_; }
private:
    std::vector<Row> rows_;
};

// ============================================================================
// Table Module — all logic for the "accounts" table
// ============================================================================
class AccountModule {
public:
    explicit AccountModule(RecordSet& rs) : rs_(rs) {}

    double total_balance() const {
        double sum = 0;
        for (const auto& r : rs_.rows()) {
            if (r.active) sum += r.balance;
        }
        return sum;
    }

    void deactivate(int id) {
        for (auto& r : rs_.rows()) {
            if (r.id == id) { r.active = false; return; }
        }
        throw std::runtime_error("not found");
    }

    void credit(int id, double amount) {
        for (auto& r : rs_.rows()) {
            if (r.id == id && r.active) { r.balance += amount; return; }
        }
        throw std::runtime_error("not found or inactive");
    }

    std::vector<Row> find_active() const {
        std::vector<Row> out;
        for (const auto& r : rs_.rows()) {
            if (r.active) out.push_back(r);
        }
        return out;
    }

private:
    RecordSet& rs_;
};

int main() {
    std::cout << "=== Table Module Pattern ===\n";
    RecordSet data;
    data.add_row({1, "Alice",  500.0, true});
    data.add_row({2, "Bob",    200.0, true});
    data.add_row({3, "Carol", 1000.0, true});

    AccountModule accounts(data);
    std::cout << "total active balance: " << accounts.total_balance() << "\n";

    accounts.deactivate(2);
    accounts.credit(1, 150);

    std::cout << "active accounts:\n";
    for (const auto& r : accounts.find_active()) {
        std::cout << "  #" << r.id << " " << r.name << " $" << r.balance << "\n";
    }
    std::cout << "total after changes: " << accounts.total_balance() << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * One module per table — logic grouped by data, not by entity\n";
    std::cout << " * Works on record sets, not individual domain objects\n";
    std::cout << " * Simpler than Domain Model, but less flexible\n";
    std::cout << " * Compare with Data Mapper + Domain Model for richer cases\n";
    return 0;
}
