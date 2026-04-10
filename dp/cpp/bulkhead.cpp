// Bulkhead — Resilience Pattern (Cloud Design Patterns)
//
// Intent: isolate resources into pools so a failure in one pool cannot
//         exhaust resources shared by all. Named after ship bulkheads
//         that contain flooding to one compartment.
//
// When to use:
//   - Multiple tenants or request types share a thread/connection pool
//   - One slow downstream should not starve others
//   - You want predictable latency per partition
//
// When NOT to use:
//   - A single homogeneous workload with no natural partitioning
//   - Resources are so scarce that partitioning wastes capacity

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>
#include <stdexcept>

// ============================================================================
// Bulkhead: a semaphore-bounded execution slot pool
// ============================================================================
class Bulkhead {
public:
    Bulkhead(std::string name, int max_concurrent)
        : name_(std::move(name)), max_(max_concurrent), current_(0) {}

    template <typename F>
    auto execute(F&& fn) -> decltype(fn()) {
        acquire();
        try {
            auto result = fn();
            release();
            return result;
        } catch (...) {
            release();
            throw;
        }
    }

    int current() const { return current_.load(); }
    const std::string& name() const { return name_; }

private:
    void acquire() {
        std::unique_lock<std::mutex> lock(mtx_);
        if (!cv_.wait_for(lock, std::chrono::milliseconds(500),
                          [this] { return current_ < max_; })) {
            throw std::runtime_error("bulkhead '" + name_ + "' full");
        }
        ++current_;
    }
    void release() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            --current_;
        }
        cv_.notify_one();
    }

    std::string             name_;
    int                     max_;
    std::atomic<int>        current_;
    std::mutex              mtx_;
    std::condition_variable cv_;
};

// ============================================================================
// Demo: two bulkheads isolate "critical" and "background" work
// ============================================================================
int main() {
    std::cout << "=== Bulkhead Pattern ===\n";
    Bulkhead critical("critical-api", 2);
    Bulkhead background("background-jobs", 3);

    auto do_work = [](Bulkhead& bh, int id, int ms) {
        try {
            bh.execute([&] {
                std::cout << "  [" << bh.name() << "] task " << id
                          << " started (slots used: " << bh.current() << ")\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                std::cout << "  [" << bh.name() << "] task " << id << " done\n";
                return 0;
            });
        } catch (const std::exception& e) {
            std::cout << "  [" << bh.name() << "] task " << id << " rejected: " << e.what() << "\n";
        }
    };

    std::vector<std::thread> threads;
    // 4 critical tasks but only 2 slots — last ones queue or timeout
    for (int i = 0; i < 4; ++i) threads.emplace_back(do_work, std::ref(critical), i, 200);
    // 5 background tasks but only 3 slots
    for (int i = 10; i < 15; ++i) threads.emplace_back(do_work, std::ref(background), i, 100);

    for (auto& t : threads) t.join();

    std::cout << "\nKey points:\n";
    std::cout << " * Each pool has a fixed concurrency limit\n";
    std::cout << " * Overflowing one pool cannot affect the other\n";
    std::cout << " * Requests that can't acquire a slot fail fast (timeout)\n";
    std::cout << " * Named after ship bulkheads that contain flooding\n";
    return 0;
}
