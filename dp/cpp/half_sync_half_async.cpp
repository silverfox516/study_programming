// Half-Sync/Half-Async — Concurrency Pattern (POSA2)
//
// Intent: separate the system into an asynchronous I/O layer (which uses
//         non-blocking calls or event notification) and a synchronous
//         processing layer (which uses blocking, easier-to-write code).
//         A thread-safe queue connects the two layers.
//
// Architecture:
//
//     [ async I/O thread ]   <- low level, non-blocking
//             |
//        [ shared queue ]    <- decouples the two halves
//             |
//     [ sync worker pool ]   <- high level, blocking, easy to write
//
// When to use:
//   - I/O is best done asynchronously (epoll, IOCP, io_uring)
//   - Business logic is easier to write synchronously
//   - You want to bound how many requests are processed concurrently
//
// When NOT to use:
//   - Both layers can be done with the same model (then just pick one)
//   - Latency-sensitive — the queue hand-off adds wakeup jitter

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <atomic>
#include <string>

// ============================================================================
// Shared queue: the seam between the two halves
// ============================================================================
template <typename T>
class BlockingQueue {
public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            q_.push(std::move(item));
        }
        cv_.notify_one();
    }
    bool pop(T& out) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !q_.empty() || closed_; });
        if (q_.empty()) return false;
        out = std::move(q_.front());
        q_.pop();
        return true;
    }
    void close() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            closed_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<T>           q_;
    std::mutex              mtx_;
    std::condition_variable cv_;
    bool                    closed_ = false;
};

// ============================================================================
// Async half: simulates an event-driven I/O layer
// ============================================================================
class AsyncReceiver {
public:
    AsyncReceiver(BlockingQueue<std::string>& queue) : queue_(queue) {}

    // In a real system this would be an epoll loop. Here we just generate
    // events on a timer.
    void run(int num_events) {
        for (int i = 0; i < num_events; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(8));
            std::string event = "request-" + std::to_string(i);
            std::cout << "[async] received " << event << "\n";
            queue_.push(std::move(event));
        }
        queue_.close();
    }

private:
    BlockingQueue<std::string>& queue_;
};

// ============================================================================
// Sync half: a small pool of workers doing blocking processing
// ============================================================================
class SyncWorkerPool {
public:
    SyncWorkerPool(BlockingQueue<std::string>& queue, std::size_t num_workers) {
        for (std::size_t i = 0; i < num_workers; ++i) {
            workers_.emplace_back([&queue, i] {
                std::string event;
                while (queue.pop(event)) {
                    // Real work — slow, blocking, but easy to write synchronously
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    std::cout << "  [worker " << i << "] processed " << event << "\n";
                }
            });
        }
    }

    ~SyncWorkerPool() {
        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }

private:
    std::vector<std::thread> workers_;
};

int main() {
    std::cout << "=== Half-Sync/Half-Async Pattern ===\n";
    BlockingQueue<std::string> queue;

    // Sync workers start first, blocking on the queue
    SyncWorkerPool workers(queue, 3);

    // Async receiver runs and feeds the queue
    AsyncReceiver receiver(queue);
    receiver.run(8);

    // workers join via destructor

    std::cout << "\nKey points:\n";
    std::cout << " * Async layer handles non-blocking I/O — high throughput\n";
    std::cout << " * Sync layer can do blocking work — simple to write\n";
    std::cout << " * Queue is the only shared state — clean isolation\n";
    std::cout << " * Each layer can be tuned (thread counts) independently\n";
    return 0;
}
