// Read/Write Lock — Concurrency Pattern (POSA2)
//
// Intent: allow concurrent readers OR exclusive write access to shared data.
//         Multiple readers can hold a shared lock at once; a writer needs an
//         exclusive lock and waits for all readers to release.
//
// When to use:
//   - Read-heavy workload with infrequent writes
//   - Read operations are non-trivial (worth the lock overhead vs. plain mutex)
//   - You can tolerate write starvation under heavy read load
//
// When NOT to use:
//   - Mostly writes — std::mutex is faster (less bookkeeping)
//   - Reads are tiny (one cache-line load) — atomics are simpler
//   - Strict write fairness required — many shared_mutex impls favor readers

#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <atomic>

// ============================================================================
// Example: thread-safe in-memory key-value store
// ============================================================================
template <typename K, typename V>
class ConcurrentMap {
public:
    // Multiple readers can call get concurrently
    bool get(const K& key, V& out) const {
        std::shared_lock<std::shared_mutex> lock(rw_);
        auto it = map_.find(key);
        if (it == map_.end()) return false;
        out = it->second;
        return true;
    }

    // Writers take an exclusive lock
    void put(const K& key, V value) {
        std::unique_lock<std::shared_mutex> lock(rw_);
        map_[key] = std::move(value);
    }

    bool erase(const K& key) {
        std::unique_lock<std::shared_mutex> lock(rw_);
        return map_.erase(key) > 0;
    }

    std::size_t size() const {
        std::shared_lock<std::shared_mutex> lock(rw_);
        return map_.size();
    }

private:
    mutable std::shared_mutex     rw_;
    std::unordered_map<K, V>      map_;
};

// ============================================================================
// Demo: many readers, occasional writers
// ============================================================================
int main() {
    std::cout << "=== Read/Write Lock Pattern ===\n";
    ConcurrentMap<std::string, int> cache;

    // Pre-populate
    for (int i = 0; i < 5; ++i) {
        cache.put("key" + std::to_string(i), i * 10);
    }

    std::atomic<long long> read_hits{0};
    std::atomic<long long> write_count{0};

    // Many readers
    auto reader = [&cache, &read_hits](int id) {
        for (int i = 0; i < 5000; ++i) {
            int v;
            if (cache.get("key" + std::to_string(i % 10), v)) {
                ++read_hits;
            }
        }
        std::cout << "  reader " << id << " done\n";
    };

    // Few writers
    auto writer = [&cache, &write_count](int id) {
        for (int i = 0; i < 50; ++i) {
            cache.put("key" + std::to_string(i % 10), id * 1000 + i);
            ++write_count;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        std::cout << "  writer " << id << " done\n";
    };

    std::vector<std::thread> threads;
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < 6; ++i) threads.emplace_back(reader, i);
    for (int i = 0; i < 2; ++i) threads.emplace_back(writer, i);
    for (auto& t : threads) t.join();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);

    std::cout << "\nresults:\n";
    std::cout << "  read_hits:  " << read_hits.load() << "\n";
    std::cout << "  writes:     " << write_count.load() << "\n";
    std::cout << "  final size: " << cache.size() << "\n";
    std::cout << "  elapsed:    " << elapsed.count() << "ms\n";

    std::cout << "\nKey points:\n";
    std::cout << " * std::shared_lock for readers, std::unique_lock for writers\n";
    std::cout << " * Reads can run in parallel — big throughput win for read-heavy workloads\n";
    std::cout << " * Writers wait for all current readers; readers wait for the writer\n";
    std::cout << " * Watch for writer starvation when reads are very frequent\n";
    return 0;
}
