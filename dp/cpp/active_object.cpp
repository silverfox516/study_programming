// Active Object — Concurrency Pattern (POSA2)
//
// Intent: decouple method invocation from method execution. A client calls a
//         method on the proxy, which packages the call as a "method request"
//         object and queues it. A scheduler thread dequeues requests and runs
//         them on the underlying servant. Results come back via futures.
//
// When to use:
//   - You want to make a class single-threaded while exposing it to many callers
//   - Method calls should not block the caller (or should block only on a future)
//   - You want to serialize access without forcing every client to take a mutex
//
// When NOT to use:
//   - Performance-critical hot paths (queue + thread switch is significant)
//   - The "servant" has no real shared state to protect
//   - Synchronous blocking access is acceptable — a plain mutex is simpler

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>
#include <chrono>
#include <atomic>
#include <string>

// ============================================================================
// The servant — only ever touched by the scheduler thread
// ============================================================================
class BankAccount {
public:
    void deposit(int amount) {
        balance_ += amount;
        std::cout << "  [servant] +"  << amount << " -> " << balance_ << "\n";
    }
    bool withdraw(int amount) {
        if (balance_ < amount) {
            std::cout << "  [servant] withdraw " << amount << " denied\n";
            return false;
        }
        balance_ -= amount;
        std::cout << "  [servant] -" << amount << " -> " << balance_ << "\n";
        return true;
    }
    int balance() const { return balance_; }

private:
    int balance_ = 0;
};

// ============================================================================
// The active object proxy
// ============================================================================
class BankAccountProxy {
public:
    BankAccountProxy() : scheduler_(&BankAccountProxy::run, this) {}

    ~BankAccountProxy() {
        {
            std::lock_guard lock(mtx_);
            stopping_ = true;
        }
        cv_.notify_all();
        scheduler_.join();
    }

    // All public methods enqueue and return a future immediately.
    std::future<void> deposit(int amount) {
        return enqueue([this, amount] { servant_.deposit(amount); });
    }

    std::future<bool> withdraw(int amount) {
        return enqueue<bool>([this, amount] { return servant_.withdraw(amount); });
    }

    std::future<int> balance() {
        return enqueue<int>([this] { return servant_.balance(); });
    }

private:
    template <typename R = void, typename F>
    std::future<R> enqueue(F&& fn) {
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(fn));
        auto fut  = task->get_future();
        {
            std::lock_guard lock(mtx_);
            queue_.emplace([task] { (*task)(); });
        }
        cv_.notify_one();
        return fut;
    }

    void run() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(mtx_);
                cv_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
                if (stopping_ && queue_.empty()) return;
                task = std::move(queue_.front());
                queue_.pop();
            }
            task();   // executed on the scheduler thread, never on the caller
        }
    }

    BankAccount                       servant_;
    std::queue<std::function<void()>> queue_;
    std::mutex                        mtx_;
    std::condition_variable           cv_;
    bool                              stopping_ = false;
    std::thread                       scheduler_;   // must be last
};

// ============================================================================
// Demo: many client threads share one active object — no client-side locking
// ============================================================================
int main() {
    std::cout << "=== Active Object Pattern ===\n";
    BankAccountProxy account;

    // Set up some money
    account.deposit(1000).get();

    // 4 client threads pummel the account concurrently
    auto client = [&account](int id) {
        for (int i = 0; i < 3; ++i) {
            if (id % 2 == 0) {
                account.deposit(50).get();
            } else {
                account.withdraw(30).get();
            }
        }
    };

    std::vector<std::thread> clients;
    for (int i = 0; i < 4; ++i) clients.emplace_back(client, i);
    for (auto& t : clients) t.join();

    int final_balance = account.balance().get();
    std::cout << "\nfinal balance: " << final_balance << "\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Servant is single-threaded — no locking inside it\n";
    std::cout << " * Proxy turns method calls into queued requests\n";
    std::cout << " * Scheduler thread executes requests one at a time\n";
    std::cout << " * Callers get futures and can choose to wait or fire-and-forget\n";
    std::cout << " * Compare with monitor_object.cpp (mutex inside the object)\n";
    return 0;
}
