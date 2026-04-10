// Double-Checked Locking — Concurrency Pattern (POSA2)
//
// Intent: amortize the cost of locking when initializing a shared resource
//         on first use. Check (lock-free) once, lock, check again, then init.
//
// When to use:
//   - Lazy singleton or one-time initialization with high read-to-init ratio
//   - You explicitly need fine-grained control beyond `static` initialization
//
// When NOT to use:
//   - C++11 magic statics (function-local `static`) already give you this for
//     free, with full thread safety. Prefer them.
//   - You can use `std::call_once` / `std::once_flag` — clearer and safer
//
// History note: pre-C++11, this pattern was famously broken because the compiler
// could reorder writes such that another thread saw a non-null pointer to a
// half-constructed object. C++11+ atomics with explicit memory orders fix it.

#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

// ============================================================================
// Example 1: The classic broken version (do NOT use)
// ============================================================================
class BrokenSingleton {
public:
    static BrokenSingleton* instance() {
        if (instance_ == nullptr) {              // unsynchronized read — UB
            std::lock_guard<std::mutex> lock(mtx_);
            if (instance_ == nullptr) {
                instance_ = new BrokenSingleton();
            }
        }
        return instance_;
    }
private:
    static BrokenSingleton* instance_;
    static std::mutex       mtx_;
};
BrokenSingleton* BrokenSingleton::instance_ = nullptr;
std::mutex       BrokenSingleton::mtx_;

// ============================================================================
// Example 2: The fixed version using std::atomic with explicit memory orders
// ============================================================================
class AtomicSingleton {
public:
    static AtomicSingleton* instance() {
        AtomicSingleton* p = instance_.load(std::memory_order_acquire);
        if (p == nullptr) {
            std::lock_guard<std::mutex> lock(mtx_);
            p = instance_.load(std::memory_order_relaxed);
            if (p == nullptr) {
                p = new AtomicSingleton();
                instance_.store(p, std::memory_order_release);
            }
        }
        return p;
    }
    int value() const { return 42; }
private:
    static std::atomic<AtomicSingleton*> instance_;
    static std::mutex                    mtx_;
};
std::atomic<AtomicSingleton*> AtomicSingleton::instance_{nullptr};
std::mutex                    AtomicSingleton::mtx_;

// ============================================================================
// Example 3: The modern way — std::call_once
// ============================================================================
class CallOnceSingleton {
public:
    static CallOnceSingleton& instance() {
        std::call_once(once_, [] { instance_.reset(new CallOnceSingleton()); });
        return *instance_;
    }
    int value() const { return 99; }
private:
    static std::once_flag                       once_;
    static std::unique_ptr<CallOnceSingleton>   instance_;
};
std::once_flag                       CallOnceSingleton::once_;
std::unique_ptr<CallOnceSingleton>   CallOnceSingleton::instance_;

// ============================================================================
// Example 4: The simplest correct way — function-local static (C++11+)
// ============================================================================
class MagicSingleton {
public:
    static MagicSingleton& instance() {
        static MagicSingleton sole;     // thread-safe init guaranteed by the standard
        return sole;
    }
    int value() const { return 7; }
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Double-Checked Locking Pattern ===\n";

    // Hammer the AtomicSingleton from many threads
    auto worker = [] {
        for (int i = 0; i < 1000; ++i) {
            AtomicSingleton* p = AtomicSingleton::instance();
            (void)p;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i) threads.emplace_back(worker);
    for (auto& t : threads) t.join();

    std::cout << "AtomicSingleton::value()    = " << AtomicSingleton::instance()->value() << "\n";
    std::cout << "CallOnceSingleton::value()  = " << CallOnceSingleton::instance().value() << "\n";
    std::cout << "MagicSingleton::value()     = " << MagicSingleton::instance().value()    << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * The plain `if (p == nullptr)` version is broken pre-C++11 — never use it\n";
    std::cout << " * Atomic load(acquire) / store(release) makes it correct in C++11+\n";
    std::cout << " * But std::call_once is clearer and harder to misuse\n";
    std::cout << " * And C++11 magic statics give you the same guarantee for free\n";
    std::cout << " * Use this pattern only when you really need fine control\n";
    return 0;
}
