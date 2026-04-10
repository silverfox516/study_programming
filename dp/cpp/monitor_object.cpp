// Monitor Object — Concurrency Pattern (POSA2)
//
// Intent: synchronize concurrent method execution by giving an object an
//         internal mutex and condition variables. Each public method takes
//         the lock on entry and waits on a condition when its precondition
//         is not met.
//
// When to use:
//   - A shared object has invariants that must hold across multiple calls
//   - Methods sometimes need to wait for state changes (use condition variables)
//   - You want callers to use the object naturally — no explicit locking outside
//
// When NOT to use:
//   - The object should appear single-threaded to its impl — use Active Object instead
//   - Lock contention dominates (consider lock-free or sharded designs)
//   - Read-mostly access — use Read/Write Lock pattern instead
//
// Compare:
//   - Active Object: queue requests, run on a dedicated thread (no caller lock)
//   - Monitor Object: caller threads acquire a lock inside the object's methods

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <atomic>

// ============================================================================
// Example 1: Bounded buffer as a monitor
// ============================================================================
template <typename T>
class MonitorBuffer {
public:
    explicit MonitorBuffer(std::size_t capacity) : capacity_(capacity) {}

    void put(T item) {
        std::unique_lock lock(mtx_);
        not_full_.wait(lock, [this] { return buffer_.size() < capacity_; });
        buffer_.push(std::move(item));
        not_empty_.notify_one();
    }

    T take() {
        std::unique_lock lock(mtx_);
        not_empty_.wait(lock, [this] { return !buffer_.empty(); });
        T item = std::move(buffer_.front());
        buffer_.pop();
        not_full_.notify_one();
        return item;
    }

    std::size_t size() const {
        std::lock_guard lock(mtx_);
        return buffer_.size();
    }

private:
    mutable std::mutex      mtx_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::queue<T>           buffer_;
    std::size_t             capacity_;
};

// ============================================================================
// Example 2: Counter monitor with wait-until-target
// ============================================================================
class WaitableCounter {
public:
    void increment() {
        std::lock_guard lock(mtx_);
        ++count_;
        cv_.notify_all();
    }

    void wait_until(int target) {
        std::unique_lock lock(mtx_);
        cv_.wait(lock, [this, target] { return count_ >= target; });
    }

    int value() const {
        std::lock_guard lock(mtx_);
        return count_;
    }

private:
    mutable std::mutex      mtx_;
    std::condition_variable cv_;
    int                     count_ = 0;
};

// ============================================================================
// Demo
// ============================================================================
void buffer_demo() {
    std::cout << "\n=== Monitor: bounded buffer ===\n";
    MonitorBuffer<int> buf(3);

    std::thread producer([&buf] {
        for (int i = 1; i <= 6; ++i) {
            buf.put(i);
            std::cout << "  put " << i << " (size=" << buf.size() << ")\n";
        }
    });

    std::thread consumer([&buf] {
        for (int i = 0; i < 6; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            int v = buf.take();
            std::cout << "  took " << v << " (size=" << buf.size() << ")\n";
        }
    });

    producer.join();
    consumer.join();
}

void counter_demo() {
    std::cout << "\n=== Monitor: waitable counter ===\n";
    WaitableCounter counter;

    // 5 worker threads bump the counter
    std::vector<std::thread> workers;
    for (int i = 0; i < 5; ++i) {
        workers.emplace_back([&counter, i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * (i + 1)));
            counter.increment();
            std::cout << "  worker " << i << " incremented\n";
        });
    }

    // One observer thread waits for at least 3 increments
    std::thread observer([&counter] {
        counter.wait_until(3);
        std::cout << "  observer woke at value=" << counter.value() << "\n";
    });

    for (auto& t : workers) t.join();
    observer.join();
    std::cout << "  final value=" << counter.value() << "\n";
}

int main() {
    std::cout << "=== Monitor Object Pattern ===\n";
    buffer_demo();
    counter_demo();

    std::cout << "\nKey points:\n";
    std::cout << " * The object owns the mutex; callers never lock externally\n";
    std::cout << " * Condition variables let methods wait for state changes\n";
    std::cout << " * Always wait with a predicate — guards against spurious wakeups\n";
    std::cout << " * Differs from Active Object by running on caller threads\n";
    return 0;
}
