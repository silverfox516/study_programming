#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <mutex>
#include <queue>

// Generic Object Pool Class
template<typename T>
class ObjectPool {
private:
    std::queue<std::unique_ptr<T>> pool;
    std::mutex mtx;
    std::function<std::unique_ptr<T>()> factory;
    size_t max_size;
    size_t in_use_count;

public:
    ObjectPool(size_t size, std::function<std::unique_ptr<T>()> factory_func)
        : max_size(size), factory(factory_func), in_use_count(0) {
        
        // Pre-warm pool
        for (size_t i = 0; i < size; ++i) {
            pool.push(factory());
        }
    }

    // Smart pointer wrapper that returns object to pool on destruction
    using PooledPtr = std::unique_ptr<T, std::function<void(T*)>>;

    PooledPtr acquire() {
        std::lock_guard<std::mutex> lock(mtx);
        
        if (pool.empty()) {
            if (in_use_count < max_size) {
                 // Should technically not happen if we pre-warm and don't expand
                 return nullptr;
            } else {
                 // Pool exhausted
                 std::cout << "[Pool] Exhausted! Retry later." << std::endl;
                 return nullptr;
            }
        }
        
        std::unique_ptr<T> obj = std::move(pool.front());
        pool.pop();
        in_use_count++;
        
        // Custom deleter using lambda to return object to pool
        return PooledPtr(obj.release(), [this](T* ptr) {
            std::lock_guard<std::mutex> lock(this->mtx);
            // Reset object state if needed here
            this->pool.push(std::unique_ptr<T>(ptr));
            this->in_use_count--;
            std::cout << "[Pool] Object returned. Available: " << this->pool.size() << std::endl;
        });
    }
    
    size_t available() const {
        // Not perfectly thread safe without lock but good for rough check
        return pool.size();
    }
};

// Example Resource
class DbConnection {
private:
    int id;
    std::string connectionString;

public:
    DbConnection(int id, std::string connStr) : id(id), connectionString(connStr) {
        std::cout << "Creating connection #" << id << std::endl;
    }
    
    void query(const std::string& sql) {
        std::cout << "Conn #" << id << " executing: " << sql << std::endl;
    }
    
    ~DbConnection() {
        std::cout << "Destroying connection #" << id << std::endl;
    }
};

int main() {
    std::cout << "=== Object Pool Pattern (C++) ===" << std::endl;

    int counter = 0;
    
    // Create a pool of 3 connections
    ObjectPool<DbConnection> pool(3, [&counter]() {
        return std::make_unique<DbConnection>(++counter, "postgres://localhost:5432");
    });

    std::cout << "\n--- Cycle 1: Acquire all ---" << std::endl;
    
    {
        auto conn1 = pool.acquire();
        if(conn1) conn1->query("SELECT * FROM users");
        
        auto conn2 = pool.acquire();
        if(conn2) conn2->query("SELECT * FROM products");
        
        auto conn3 = pool.acquire();
        
        auto conn4 = pool.acquire(); // Should fail
        if (!conn4) std::cout << "Could not acquire conn4" << std::endl;
        
        // End of scope: conn1, conn2, conn3 are returned automatically!
    }
    
    std::cout << "\n--- Cycle 2: Reuse ---" << std::endl;
    
    auto conn5 = pool.acquire(); // Should succeed reusing an object
    if (conn5) conn5->query("SELECT NOW()");

    return 0;
}
