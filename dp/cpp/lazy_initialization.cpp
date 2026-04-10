// Lazy Initialization — Concurrency / Creational Pattern
//
// Intent: defer the creation of an expensive resource until it is first used,
//         and ensure it is only created once even under concurrent access.
//
// When to use:
//   - The resource is costly to build and may never be needed
//   - Thread safety is required (singleton, shared caches, connection pools)
//
// When NOT to use:
//   - The resource is cheap — just construct it eagerly
//   - Startup latency hiding is acceptable — eager init keeps runtime predictable

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

class Database {
public:
    Database() {
        std::cout << "  [Database] opening connection (expensive!)...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    void query(const std::string& q) const {
        std::cout << "  [Database] " << q << "\n";
    }
};

// ============================================================================
// Example 1: std::call_once — the canonical modern approach
// ============================================================================
class LazyCallOnce {
public:
    Database& get() {
        std::call_once(flag_, [this] { db_ = std::make_unique<Database>(); });
        return *db_;
    }
private:
    std::once_flag              flag_;
    std::unique_ptr<Database>   db_;
};

// ============================================================================
// Example 2: function-local static — simplest thread-safe lazy
// ============================================================================
Database& get_global_db() {
    static Database db;   // thread-safe construction guaranteed by C++11
    return db;
}

// ============================================================================
// Example 3: std::optional for member-level lazy without pointer overhead
// ============================================================================
class Service {
public:
    void do_work() {
        if (!cache_) {
            cache_.emplace();    // default-construct on first use
            cache_->push_back("initialized");
        }
        cache_->push_back("used");
        std::cout << "  cache size: " << cache_->size() << "\n";
    }
private:
    std::optional<std::vector<std::string>> cache_;
};

int main() {
    std::cout << "=== Lazy Initialization Pattern ===\n";

    std::cout << "\n--- call_once ---\n";
    LazyCallOnce lazy;
    std::vector<std::thread> ts;
    for (int i = 0; i < 4; ++i) {
        ts.emplace_back([&lazy, i] {
            lazy.get().query("query from thread " + std::to_string(i));
        });
    }
    for (auto& t : ts) t.join();

    std::cout << "\n--- function-local static ---\n";
    get_global_db().query("select 1");

    std::cout << "\n--- optional ---\n";
    Service svc;
    svc.do_work();
    svc.do_work();

    std::cout << "\nKey points:\n";
    std::cout << " * call_once: explicit, works for member-level lazy init\n";
    std::cout << " * static local: simplest for global/singleton resources\n";
    std::cout << " * optional: avoids heap allocation for deferred member objects\n";
    return 0;
}
