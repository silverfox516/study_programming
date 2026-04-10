// Transaction Script — Enterprise Pattern (PoEAA)
//
// Intent: organize business logic as a single procedure per user action.
//         Each "script" handles one request from start to finish —
//         validation, computation, persistence — in a procedural style.
//
// When to use:
//   - Business logic is simple — one or two conditionals, no complex domain rules
//   - CRUD apps, admin panels, batch jobs with straightforward flows
//   - Getting started quickly before investing in a domain model
//
// When NOT to use:
//   - Logic grows complex — scripts start duplicating each other
//   - Multiple scripts need to share domain rules — extract a Domain Model
//
// Why study it:
//   Transaction Script is the *simplest* organization of business logic.
//   Knowing it is the baseline helps you recognize when you've outgrown it.

#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>

// ============================================================================
// A trivial "database"
// ============================================================================
struct Account {
    std::string owner;
    double      balance;
};

std::unordered_map<int, Account> db = {
    {1, {"Alice", 500.0}},
    {2, {"Bob",   300.0}},
};

// ============================================================================
// Transaction Scripts — each one is a complete use case
// ============================================================================
void transfer_funds(int from_id, int to_id, double amount) {
    std::cout << "--- transfer " << amount << " from #" << from_id
              << " to #" << to_id << " ---\n";

    if (db.find(from_id) == db.end()) throw std::runtime_error("source not found");
    if (db.find(to_id)   == db.end()) throw std::runtime_error("target not found");
    if (amount <= 0)                  throw std::invalid_argument("amount must be positive");
    if (db[from_id].balance < amount) throw std::runtime_error("insufficient funds");

    db[from_id].balance -= amount;
    db[to_id].balance   += amount;

    std::cout << "  " << db[from_id].owner << " balance: " << db[from_id].balance << "\n";
    std::cout << "  " << db[to_id].owner   << " balance: " << db[to_id].balance   << "\n";
}

void close_account(int id) {
    std::cout << "--- close account #" << id << " ---\n";

    auto it = db.find(id);
    if (it == db.end()) throw std::runtime_error("not found");
    if (it->second.balance != 0.0) {
        std::cout << "  cannot close — balance is " << it->second.balance << "\n";
        return;
    }
    db.erase(it);
    std::cout << "  closed\n";
}

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Transaction Script Pattern ===\n";

    transfer_funds(1, 2, 100.0);

    try {
        transfer_funds(1, 2, 9999.0);
    } catch (const std::exception& e) {
        std::cout << "  failed: " << e.what() << "\n";
    }

    // Drain Alice's balance so we can close the account
    transfer_funds(1, 2, 400.0);
    close_account(1);

    std::cout << "\nKey points:\n";
    std::cout << " * Each script = one complete use case (validate -> compute -> persist)\n";
    std::cout << " * No domain objects — logic is procedural\n";
    std::cout << " * Works great for simple apps; becomes spaghetti when logic grows\n";
    std::cout << " * The stepping stone to Domain Model and Service Layer\n";
    return 0;
}
