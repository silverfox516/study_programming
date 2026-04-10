// Thread Pool — Concurrency Pattern
//
// Intent: maintain a fixed (or bounded) set of worker threads that execute tasks
//         submitted to a shared queue. Avoids the overhead of spawning a thread
//         per task and bounds parallelism explicitly.
//
// When to use:
//   - You have many short tasks
//   - You want to bound the number of concurrent threads (CPU saturation control)
//   - You want futures/promises so callers can wait for results
//
// When NOT to use:
//   - Tasks are long-lived blocking I/O — use std::async or coroutines instead
//   - Single big batch job — std::for_each(par_unseq, ...) may be simpler
//   - Tasks have rich dependencies — a task graph executor is a better fit

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <future>
#include <functional>
#include <atomic>
#include <chrono>

// ============================================================================
// Example 1: Generic thread pool with std::future-returning submit
// ============================================================================
class ThreadPool {
public:
    explicit ThreadPool(std::size_t num_threads) {
        for (std::size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() { shutdown(); }

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Submit a callable returning R; the caller gets a future for the result.
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using R = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<R()>>(
            [f = std::forward<F>(f),
             args = std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...)]() mutable {
                return std::apply(std::move(f), std::move(args));
            });

        std::future<R> result = task->get_future();
        {
            std::lock_guard lock(mtx_);
            if (stopping_) throw std::runtime_error("pool stopped");
            tasks_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return result;
    }

    void shutdown() {
        {
            std::lock_guard lock(mtx_);
            if (stopping_) return;
            stopping_ = true;
        }
        cv_.notify_all();
        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }

    std::size_t size() const { return workers_.size(); }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(mtx_);
                cv_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });
                if (stopping_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::thread>          workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex                        mtx_;
    std::condition_variable           cv_;
    bool                              stopping_ = false;
};

// ============================================================================
// Example 2: CPU-bound parallel sum via the pool
// ============================================================================
long long parallel_sum(const std::vector<int>& data, ThreadPool& pool, std::size_t chunks) {
    std::vector<std::future<long long>> partials;
    std::size_t chunk_size = data.size() / chunks;

    for (std::size_t c = 0; c < chunks; ++c) {
        std::size_t begin = c * chunk_size;
        std::size_t end   = (c == chunks - 1) ? data.size() : begin + chunk_size;
        partials.push_back(pool.submit([&data, begin, end]() {
            long long sum = 0;
            for (std::size_t i = begin; i < end; ++i) sum += data[i];
            return sum;
        }));
    }

    long long total = 0;
    for (auto& f : partials) total += f.get();
    return total;
}

// ============================================================================
// Example 3: Independent fire-and-forget tasks via void return
// ============================================================================
void run_logging_demo(ThreadPool& pool) {
    std::cout << "\n=== Fire-and-forget logging tasks ===\n";
    std::atomic<int> processed{0};

    for (int i = 0; i < 8; ++i) {
        pool.submit([i, &processed] {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout << "  task " << i
                      << " done by thread " << std::this_thread::get_id() << "\n";
            ++processed;
        });
    }

    // Drain by waiting for the count
    while (processed.load() < 8) std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

int main() {
    std::cout << "=== Thread Pool Pattern ===\n";

    const unsigned hw = std::max(2u, std::thread::hardware_concurrency());
    ThreadPool pool(hw);
    std::cout << "pool size: " << pool.size() << "\n";

    // Parallel sum demo
    std::vector<int> data;
    for (int i = 1; i <= 10000; ++i) data.push_back(i);
    long long expected = static_cast<long long>(10000) * 10001 / 2;

    auto start = std::chrono::steady_clock::now();
    long long result = parallel_sum(data, pool, 8);
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);

    std::cout << "parallel sum:    " << result << "\n";
    std::cout << "expected:        " << expected << "\n";
    std::cout << "time (us):       " << elapsed.count() << "\n";

    run_logging_demo(pool);

    pool.shutdown();

    std::cout << "\nKey points:\n";
    std::cout << " * Bounded parallelism: pool size caps concurrent threads\n";
    std::cout << " * Tasks are queued, workers pull from a single shared queue\n";
    std::cout << " * Futures let callers retrieve results or exceptions\n";
    std::cout << " * shutdown() drains and joins all workers cleanly\n";
    return 0;
}
