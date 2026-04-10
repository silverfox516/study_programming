// Producer-Consumer (Bounded Blocking Queue) — Concurrency Pattern (POSA2)
//
// Intent: decouple producers and consumers by inserting a thread-safe buffer
//         between them. Producers block when the buffer is full; consumers
//         block when it is empty.
//
// When to use:
//   - Different production and consumption rates need to be smoothed
//   - Multiple producers / consumers should work in parallel safely
//   - Backpressure is desired (slow consumer naturally throttles producer)
//
// When NOT to use:
//   - Strict ordering across producers is critical (this only orders within one queue)
//   - Latency is more important than throughput (the queue adds wake-up jitter)
//   - You can use a lock-free SPSC ring buffer for the single-producer/single-consumer case

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <optional>
#include <string>

// ============================================================================
// Example 1: Generic bounded blocking queue
// ============================================================================
template <typename T>
class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(std::size_t capacity) : capacity_(capacity) {}

    // Producers call push; blocks if the queue is full
    void push(T value) {
        std::unique_lock lock(mtx_);
        not_full_.wait(lock, [this] { return queue_.size() < capacity_ || closed_; });
        if (closed_) return;
        queue_.push(std::move(value));
        not_empty_.notify_one();
    }

    // Consumers call pop; blocks if the queue is empty.
    // Returns std::nullopt only when the queue is closed AND drained.
    std::optional<T> pop() {
        std::unique_lock lock(mtx_);
        not_empty_.wait(lock, [this] { return !queue_.empty() || closed_; });
        if (queue_.empty()) return std::nullopt;
        T value = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return value;
    }

    // Closing the queue wakes everyone so consumers can exit cleanly
    void close() {
        std::lock_guard lock(mtx_);
        closed_ = true;
        not_empty_.notify_all();
        not_full_.notify_all();
    }

    std::size_t size() const {
        std::lock_guard lock(mtx_);
        return queue_.size();
    }

private:
    mutable std::mutex      mtx_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
    std::queue<T>           queue_;
    std::size_t             capacity_;
    bool                    closed_ = false;
};

// ============================================================================
// Example 2: Multi-producer / multi-consumer pipeline
// ============================================================================
struct WorkItem {
    int         id;
    std::string payload;
};

void run_pipeline_demo() {
    std::cout << "\n=== Multi-producer / multi-consumer pipeline ===\n";
    BoundedBlockingQueue<WorkItem> queue(5);

    constexpr int num_producers = 2;
    constexpr int num_consumers = 3;
    constexpr int items_per_producer = 6;
    std::atomic<int> consumed_count{0};

    // Producers
    std::vector<std::thread> producers;
    for (int p = 0; p < num_producers; ++p) {
        producers.emplace_back([&queue, p]() {
            for (int i = 0; i < items_per_producer; ++i) {
                WorkItem item{p * 100 + i, "from producer " + std::to_string(p)};
                queue.push(item);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    // Consumers
    std::vector<std::thread> consumers;
    for (int c = 0; c < num_consumers; ++c) {
        consumers.emplace_back([&queue, &consumed_count, c]() {
            while (auto item = queue.pop()) {
                std::cout << "consumer " << c << " got id=" << item->id
                          << " (" << item->payload << ")\n";
                ++consumed_count;
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        });
    }

    // Wait for producers to finish, then close the queue
    for (auto& t : producers) t.join();
    queue.close();

    // Drain consumers
    for (auto& t : consumers) t.join();

    std::cout << "total consumed: " << consumed_count.load() << "\n";
}

// ============================================================================
// Example 3: Backpressure demonstration — slow consumer throttles fast producer
// ============================================================================
void run_backpressure_demo() {
    std::cout << "\n=== Backpressure: slow consumer throttles fast producer ===\n";
    BoundedBlockingQueue<int> queue(3);    // small capacity

    auto start = std::chrono::steady_clock::now();
    auto elapsed_ms = [&] {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count();
    };

    std::thread producer([&] {
        for (int i = 1; i <= 6; ++i) {
            queue.push(i);
            std::cout << "[" << elapsed_ms() << "ms] produced " << i << "\n";
        }
        queue.close();
    });

    std::thread consumer([&] {
        // Deliberately slow — producer must block when the queue fills
        while (auto v = queue.pop()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::cout << "[" << elapsed_ms() << "ms] consumed " << *v << "\n";
        }
    });

    producer.join();
    consumer.join();
}

int main() {
    std::cout << "=== Producer-Consumer Pattern ===\n";
    run_pipeline_demo();
    run_backpressure_demo();

    std::cout << "\nKey points:\n";
    std::cout << " * push/pop are protected by a single mutex\n";
    std::cout << " * not_full / not_empty condition variables prevent busy-waiting\n";
    std::cout << " * close() lets consumers exit cleanly without sentinel values\n";
    std::cout << " * Capacity creates backpressure on producers automatically\n";
    return 0;
}
