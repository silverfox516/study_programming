// Proactor — Concurrency Pattern (POSA2)
//
// Intent: like Reactor, but the *operation* runs asynchronously and the handler
//         is invoked when it *completes*, not when it is ready to start.
//         Reactor: "this socket is readable, call me to read it."
//         Proactor: "I started a read for you, call me when the bytes are here."
//
// When to use:
//   - You want to overlap I/O with computation
//   - The OS supports async-completion APIs (IOCP on Windows, io_uring on Linux,
//     POSIX aio_*)
//   - You want simpler handler code than Reactor's "loop until EAGAIN" pattern
//
// When NOT to use:
//   - Cross-platform code where async APIs differ wildly (Reactor is more portable)
//   - Tasks are tiny (the completion-callback overhead dominates)
//
// This in-process simulation uses a worker thread to "complete" requests
// asynchronously and post completion events back to the proactor's queue.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <chrono>
#include <string>

// ============================================================================
// Proactor with completion-handler dispatch
// ============================================================================
class Proactor {
public:
    using CompletionHandler = std::function<void(const std::string& result)>;

    Proactor() : worker_(&Proactor::worker_loop, this) {}

    ~Proactor() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stopping_ = true;
        }
        cv_.notify_all();
        worker_.join();
    }

    // Initiate an async "read" — returns immediately, calls handler later
    void async_read(const std::string& source, CompletionHandler on_complete) {
        std::lock_guard<std::mutex> lock(mtx_);
        operations_.push({source, std::move(on_complete)});
        cv_.notify_one();
    }

    // The application's main loop drains completion events
    void run() {
        std::cout << "[proactor] running event loop\n";
        while (true) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return !completions_.empty() || (stopping_ && operations_.empty()); });

            if (completions_.empty() && stopping_ && operations_.empty()) break;
            if (completions_.empty()) continue;

            auto [handler, result] = std::move(completions_.front());
            completions_.pop();
            lock.unlock();

            handler(result);   // dispatch on the application thread
        }
        std::cout << "[proactor] event loop done\n";
    }

private:
    struct Operation {
        std::string       source;
        CompletionHandler on_complete;
    };
    struct Completion {
        CompletionHandler handler;
        std::string       result;
    };

    void worker_loop() {
        while (true) {
            Operation op;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this] { return !operations_.empty() || stopping_; });
                if (operations_.empty() && stopping_) return;
                op = std::move(operations_.front());
                operations_.pop();
            }

            // Pretend we did a real async I/O — simulated with a sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::string result = "data from " + op.source;

            {
                std::lock_guard<std::mutex> lock(mtx_);
                completions_.push({std::move(op.on_complete), std::move(result)});
            }
            cv_.notify_all();
        }
    }

    std::queue<Operation>   operations_;    // pending async ops
    std::queue<Completion>  completions_;   // ready-to-dispatch results
    std::mutex              mtx_;
    std::condition_variable cv_;
    bool                    stopping_ = false;
    std::thread             worker_;
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Proactor Pattern ===\n";
    Proactor proactor;

    std::atomic<int> remaining{3};

    proactor.async_read("file.txt", [&](const std::string& r) {
        std::cout << "  handler 1 got: " << r << "\n";
        --remaining;
    });

    proactor.async_read("socket-9000", [&](const std::string& r) {
        std::cout << "  handler 2 got: " << r << "\n";
        --remaining;
    });

    proactor.async_read("pipe-foo", [&](const std::string& r) {
        std::cout << "  handler 3 got: " << r << "\n";
        --remaining;
    });

    // Drain on the main thread
    std::thread runner([&] { proactor.run(); });

    while (remaining > 0) std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // Stopping is destructor-driven via worker_loop seeing stopping_=true
    // We need to wake the runner; easy way is destruction in the right order.
    // For demo purposes, give it a moment then exit.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // proactor's destructor will set stopping_ and join the worker; the runner
    // thread will exit when it sees the stopping condition.
    runner.detach();   // let the destructor finish things off

    std::cout << "\nKey points:\n";
    std::cout << " * Operation initiation and completion are decoupled in time\n";
    std::cout << " * Handler runs on the application thread, not the I/O thread\n";
    std::cout << " * Real proactors use OS facilities: IOCP, io_uring, POSIX aio\n";
    std::cout << " * Compare with Reactor: Reactor signals readiness, Proactor signals completion\n";
    return 0;
}
