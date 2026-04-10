// Cache-Aside — Resilience / Performance Pattern
//
// Intent: the application checks the cache before querying the data store.
//         On a miss it fetches from the store, puts it in the cache, then
//         returns the value. Writes go directly to the store, and the cache
//         entry is invalidated.
//
// When to use:
//   - Read-heavy workloads with tolerable staleness
//   - The cache is separate from the data store (Redis, Memcached)
//   - You want full control over cache population and eviction
//
// When NOT to use:
//   - Write-heavy — cache invalidation thrashing kills throughput
//   - Strong consistency required — stale reads are unacceptable
//   - The data store already has a built-in caching layer

#include <iostream>
#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <thread>

// ============================================================================
// Simulated data store and cache
// ============================================================================
class DataStore {
public:
    DataStore() {
        data_["user:1"] = "Alice";
        data_["user:2"] = "Bob";
        data_["user:3"] = "Carol";
    }
    std::optional<std::string> get(const std::string& key) const {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // slow
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;
        return it->second;
    }
    void put(const std::string& key, const std::string& value) {
        data_[key] = value;
    }
private:
    std::unordered_map<std::string, std::string> data_;
};

class Cache {
public:
    std::optional<std::string> get(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;
        return it->second;
    }
    void put(const std::string& key, const std::string& value) {
        data_[key] = value;
    }
    void invalidate(const std::string& key) {
        data_.erase(key);
    }
private:
    std::unordered_map<std::string, std::string> data_;
};

// ============================================================================
// Cache-aside service
// ============================================================================
class UserService {
public:
    UserService(DataStore& store, Cache& cache) : store_(store), cache_(cache) {}

    std::string get_user(const std::string& key) {
        // 1. Check cache
        if (auto v = cache_.get(key)) {
            std::cout << "  [cache HIT]  " << key << "\n";
            return *v;
        }
        // 2. Cache miss — fetch from store
        std::cout << "  [cache MISS] " << key << " — fetching from store\n";
        auto v = store_.get(key);
        if (!v) return "(not found)";
        // 3. Populate cache
        cache_.put(key, *v);
        return *v;
    }

    void update_user(const std::string& key, const std::string& value) {
        store_.put(key, value);
        cache_.invalidate(key);   // invalidate — next read will re-fetch
        std::cout << "  [write-through] " << key << " updated, cache invalidated\n";
    }

private:
    DataStore& store_;
    Cache&     cache_;
};

int main() {
    std::cout << "=== Cache-Aside Pattern ===\n";
    DataStore store;
    Cache     cache;
    UserService svc(store, cache);

    svc.get_user("user:1");   // miss
    svc.get_user("user:1");   // hit
    svc.get_user("user:2");   // miss

    svc.update_user("user:1", "Alice Updated");
    svc.get_user("user:1");   // miss again — invalidated

    std::cout << "\nKey points:\n";
    std::cout << " * Application controls cache population — not the store\n";
    std::cout << " * Writes invalidate the cache; next read re-populates\n";
    std::cout << " * Stale data possible if another process writes to the store\n";
    std::cout << " * Pair with TTL-based eviction for time-bounded staleness\n";
    return 0;
}
