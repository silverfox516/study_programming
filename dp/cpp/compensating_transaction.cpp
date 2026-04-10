// Compensating Transaction — Cloud / DDD Pattern
//
// Intent: undo a completed operation that cannot be rolled back via a
//         traditional ACID transaction. Instead of rollback, a compensating
//         action reverses the effect semantically.
//
// When to use:
//   - Distributed systems where multi-step operations span services/databases
//   - The "undo" is a domain operation (refund, cancel reservation, restore quota)
//   - Pairs with Saga (dp/cpp/saga.cpp) which orchestrates multiple compensating steps
//
// When NOT to use:
//   - The operation can be wrapped in a single ACID transaction
//   - Compensation is undefined (e.g., you already sent a physical package)

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <memory>

// ============================================================================
// Action + its compensation, as a pair
// ============================================================================
class CompensableAction {
public:
    CompensableAction(std::string name,
                      std::function<void()> execute,
                      std::function<void()> compensate)
        : name_(std::move(name)), execute_(std::move(execute)),
          compensate_(std::move(compensate)) {}

    void execute()    const { execute_(); }
    void compensate() const { compensate_(); }
    const std::string& name() const { return name_; }

private:
    std::string             name_;
    std::function<void()>   execute_;
    std::function<void()>   compensate_;
};

// ============================================================================
// Transaction runner — executes forward, compensates backward on failure
// ============================================================================
class CompensatingTransaction {
public:
    void add(CompensableAction action) {
        actions_.push_back(std::move(action));
    }

    bool run() {
        std::vector<int> completed;
        for (int i = 0; i < static_cast<int>(actions_.size()); ++i) {
            try {
                std::cout << "  [exec] " << actions_[i].name() << "\n";
                actions_[i].execute();
                completed.push_back(i);
            } catch (const std::exception& e) {
                std::cout << "  [FAIL] " << actions_[i].name() << ": " << e.what() << "\n";
                compensate_all(completed);
                return false;
            }
        }
        return true;
    }

private:
    void compensate_all(const std::vector<int>& completed) {
        std::cout << "  [compensating...]\n";
        for (auto it = completed.rbegin(); it != completed.rend(); ++it) {
            std::cout << "  [undo] " << actions_[*it].name() << "\n";
            try { actions_[*it].compensate(); }
            catch (const std::exception& e) {
                std::cout << "  [WARN] compensation failed: " << e.what() << "\n";
            }
        }
    }

    std::vector<CompensableAction> actions_;
};

// ============================================================================
// Demo: e-commerce checkout with compensation
// ============================================================================
int main() {
    std::cout << "=== Compensating Transaction Pattern ===\n";

    double wallet = 500.0;
    int    stock  = 10;
    bool   reserved_shipping = false;

    auto debit_wallet = CompensableAction(
        "debit wallet $200",
        [&] { if (wallet < 200) throw std::runtime_error("insufficient funds");
              wallet -= 200; },
        [&] { wallet += 200; std::cout << "    refunded $200\n"; });

    auto reserve_stock = CompensableAction(
        "reserve 2 items",
        [&] { if (stock < 2) throw std::runtime_error("out of stock");
              stock -= 2; },
        [&] { stock += 2; std::cout << "    stock released\n"; });

    auto book_shipping = CompensableAction(
        "book shipping",
        [&] { throw std::runtime_error("carrier unavailable"); },  // fails
        [&] { reserved_shipping = false; });

    CompensatingTransaction txn;
    txn.add(debit_wallet);
    txn.add(reserve_stock);
    txn.add(book_shipping);

    bool ok = txn.run();
    std::cout << "\ntransaction " << (ok ? "succeeded" : "failed + compensated") << "\n";
    std::cout << "  wallet=" << wallet << " stock=" << stock << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Each action defines its own compensation\n";
    std::cout << " * On failure, completed steps are undone in reverse\n";
    std::cout << " * Compensation is semantic (refund), not physical (rollback)\n";
    std::cout << " * See saga.cpp for orchestrating multiple compensating transactions\n";
    return 0;
}
