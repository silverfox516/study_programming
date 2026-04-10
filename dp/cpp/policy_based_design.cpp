// Policy-Based Design — C++ Idiom (Alexandrescu, "Modern C++ Design")
//
// Intent: decompose a class into independent, interchangeable "policy" template
//         parameters. Each policy handles one dimension of behavior. The host
//         class assembles policies by inheriting from or containing them.
//
// When to use:
//   - Multiple independent axes of variation (allocation, threading, logging)
//   - You want static (compile-time) configuration — no virtual dispatch
//   - Library design where users need to customize behavior without forking
//
// When NOT to use:
//   - The axes are not truly independent — interactions between policies explode
//   - Runtime selection is required — use Strategy or DI instead
//   - The policy set is tiny — just use template specialization

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <memory>

// ============================================================================
// Policies: Threading
// ============================================================================
struct SingleThreaded {
    struct Lock { Lock() = default; };   // no-op lock
};

struct MultiThreaded {
    struct Lock {
        Lock() { mtx_.lock(); }
        ~Lock() { mtx_.unlock(); }
        Lock(const Lock&)            = delete;
        Lock& operator=(const Lock&) = delete;
    private:
        static inline std::mutex mtx_;
    };
};

// ============================================================================
// Policies: Storage
// ============================================================================
template <typename T>
struct HeapStorage {
    using Ptr = std::unique_ptr<T>;
    static Ptr create(T value) { return std::make_unique<T>(std::move(value)); }
    static T&  deref(const Ptr& p) { return *p; }
};

template <typename T>
struct InlineStorage {
    using Ptr = T;
    static Ptr create(T value) { return value; }
    static T&  deref(Ptr& p) { return p; }
    static const T& deref(const Ptr& p) { return p; }
};

// ============================================================================
// Policies: Logging
// ============================================================================
struct NoLogging {
    static void log(const std::string&) {}
};

struct ConsoleLogging {
    static void log(const std::string& msg) {
        std::cout << "  [log] " << msg << "\n";
    }
};

// ============================================================================
// Host class: assembles policies
// ============================================================================
template <
    typename T,
    template <typename> class StoragePolicy = HeapStorage,
    typename ThreadingPolicy = SingleThreaded,
    typename LoggingPolicy   = NoLogging
>
class SmartContainer {
public:
    void add(T value) {
        [[maybe_unused]] typename ThreadingPolicy::Lock lock;
        LoggingPolicy::log("add");
        items_.push_back(StoragePolicy<T>::create(std::move(value)));
    }

    const T& get(std::size_t i) const {
        return StoragePolicy<T>::deref(items_[i]);
    }

    std::size_t size() const { return items_.size(); }

private:
    std::vector<typename StoragePolicy<T>::Ptr> items_;
};

// ============================================================================
// Demo: different policy combinations
// ============================================================================
int main() {
    std::cout << "=== Policy-Based Design Pattern ===\n";

    // Default: heap storage, single-threaded, no logging
    SmartContainer<std::string> basic;
    basic.add("hello");
    basic.add("world");
    std::cout << "basic[0] = " << basic.get(0) << "\n";

    // Multi-threaded + console logging + inline storage
    SmartContainer<int, InlineStorage, MultiThreaded, ConsoleLogging> fancy;
    fancy.add(42);
    fancy.add(99);
    std::cout << "fancy[1] = " << fancy.get(1) << "\n";
    std::cout << "fancy size = " << fancy.size() << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Each policy is an independent template parameter\n";
    std::cout << " * Policies compose orthogonally — N*M*K combinations from N+M+K classes\n";
    std::cout << " * All dispatch is static — no virtual overhead\n";
    std::cout << " * The host class is lean — just wires policies together\n";
    std::cout << " * Compare with Strategy (runtime swap) and CRTP (static mixin)\n";
    return 0;
}
