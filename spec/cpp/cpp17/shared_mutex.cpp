#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <mutex>

class ThreadSafeCounter {
public:
    ThreadSafeCounter() = default;

    // Multiple threads can read concurrently
    unsigned int get() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }

    // Only one thread can write
    void increment() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_++;
    }

    // Only one thread can write
    void reset() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ = 0;
    }

private:
    mutable std::shared_mutex mutex_;
    unsigned int value_ = 0;
};

void reader(ThreadSafeCounter& counter) {
    for (int i = 0; i < 100; ++i) {
        // Just reading
        unsigned int val = counter.get(); 
    }
}

void writer(ThreadSafeCounter& counter) {
    for (int i = 0; i < 100; ++i) {
        counter.increment();
    }
}

int main() {
    ThreadSafeCounter counter;
    std::vector<std::thread> threads;

    // 5 writers
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(writer, std::ref(counter));
    }

    // 10 readers
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(reader, std::ref(counter));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final counter value: " << counter.get() << std::endl;

    return 0;
}
