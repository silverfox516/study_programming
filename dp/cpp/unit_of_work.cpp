// Unit of Work — Enterprise Pattern (PoEAA)
//
// Intent: tracks objects affected by a business transaction and coordinates
//         writing out the changes (and resolving concurrency issues).
//         The "unit" is committed atomically — all or nothing.
//
// When to use:
//   - You have a transactional boundary (HTTP request, message handler)
//   - Multiple repositories need to commit together
//   - You want to defer writes until you know the whole operation is valid
//
// When NOT to use:
//   - Single-table CRUD — the abstraction adds noise
//   - The underlying store has no transactions (then the pattern is a lie)

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>
#include <stdexcept>

// ============================================================================
// Domain entity
// ============================================================================
struct Customer {
    int         id;
    std::string name;
    double      balance;
};

// ============================================================================
// A pretend "database" — the actual storage we ultimately commit to
// ============================================================================
class Database {
public:
    void insert(const Customer& c) {
        if (rows_.count(c.id)) throw std::runtime_error("duplicate id");
        rows_[c.id] = c;
        std::cout << "  [db] INSERT id=" << c.id << " name=" << c.name << "\n";
    }
    void update(const Customer& c) {
        if (!rows_.count(c.id)) throw std::runtime_error("not found");
        rows_[c.id] = c;
        std::cout << "  [db] UPDATE id=" << c.id << " balance=" << c.balance << "\n";
    }
    void erase(int id) {
        rows_.erase(id);
        std::cout << "  [db] DELETE id=" << id << "\n";
    }
    Customer get(int id) const { return rows_.at(id); }
    std::size_t size() const   { return rows_.size(); }

private:
    std::unordered_map<int, Customer> rows_;
};

// ============================================================================
// Unit of Work — accumulates changes, commits atomically
// ============================================================================
class UnitOfWork {
public:
    explicit UnitOfWork(Database& db) : db_(db) {}

    void register_new(const Customer& c)     { new_.push_back(c); }
    void register_dirty(const Customer& c)   { dirty_.push_back(c); }
    void register_deleted(int id)            { deleted_.push_back(id); }

    // Apply all pending changes. If any step throws, the demo simulates
    // rollback by clearing pending state — a real impl would use a real
    // database transaction here.
    void commit() {
        std::cout << "  [uow] COMMIT (" << new_.size() << " new, "
                  << dirty_.size() << " dirty, "
                  << deleted_.size() << " deleted)\n";
        try {
            for (const auto& c : new_)     db_.insert(c);
            for (const auto& c : dirty_)   db_.update(c);
            for (int id : deleted_)        db_.erase(id);
        } catch (...) {
            std::cout << "  [uow] commit failed — rolling back\n";
            rollback();
            throw;
        }
        clear();
    }

    void rollback() { clear(); }

private:
    void clear() {
        new_.clear();
        dirty_.clear();
        deleted_.clear();
    }

    Database&             db_;
    std::vector<Customer> new_;
    std::vector<Customer> dirty_;
    std::vector<int>      deleted_;
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Unit of Work Pattern ===\n";
    Database db;

    // Successful unit
    {
        UnitOfWork uow(db);
        uow.register_new(Customer{1, "Alice", 100.0});
        uow.register_new(Customer{2, "Bob",    50.0});
        uow.commit();
    }
    std::cout << "  db now has " << db.size() << " rows\n";

    // Mutating unit
    {
        UnitOfWork uow(db);
        Customer alice = db.get(1);
        alice.balance += 75.0;
        uow.register_dirty(alice);
        uow.register_deleted(2);
        uow.commit();
    }
    std::cout << "  db now has " << db.size() << " rows\n";

    // Failing unit — duplicate id triggers rollback
    try {
        UnitOfWork uow(db);
        uow.register_new(Customer{3, "Carol", 200.0});
        uow.register_new(Customer{1, "Duplicate", 0.0});  // boom
        uow.commit();
    } catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }
    std::cout << "  db still has " << db.size() << " rows after rollback\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Changes are recorded but not applied until commit()\n";
    std::cout << " * commit() is atomic — all or nothing\n";
    std::cout << " * Caller code is dead simple — just register intent\n";
    std::cout << " * Real impls wrap a real DB transaction inside commit()\n";
    return 0;
}
