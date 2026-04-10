// Throttling (Rate Limiting) — Resilience Pattern
//
// Intent: limit the rate of operations to protect a downstream resource
//         or enforce a usage policy. Excess requests are rejected or queued.
//
// When to use:
//   - API rate limits (e.g. 100 requests per second)
//   - Preventing self-inflicted DDoS on a database or service
//   - Fair usage among tenants
//
// When NOT to use:
//   - All requests must be served regardless of rate — use queueing instead
//   - The bottleneck is CPU, not I/O — use backpressure (bounded queue)

#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

// ============================================================================
// Token bucket rate limiter
// ============================================================================
class TokenBucket {
public:
    TokenBucket(int capacity, int refill_per_second)
        : capacity_(capacity), tokens_(capacity),
          refill_interval_(std::chrono::seconds(1) / refill_per_second),
          last_refill_(std::chrono::steady_clock::now()) {}

    bool try_acquire() {
        std::lock_guard<std::mutex> lock(mtx_);
        refill();
        if (tokens_ > 0) { --tokens_; return true; }
        return false;
    }

    int available() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return tokens_;
    }

private:
    void refill() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - last_refill_;
        int new_tokens = static_cast<int>(elapsed / refill_interval_);
        if (new_tokens > 0) {
            tokens_ = std::min(capacity_, tokens_ + new_tokens);
            last_refill_ = now;
        }
    }

    int                                      capacity_;
    int                                      tokens_;
    std::chrono::steady_clock::duration      refill_interval_;
    std::chrono::steady_clock::time_point    last_refill_;
    mutable std::mutex                       mtx_;
};

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Throttling (Rate Limiting) Pattern ===\n";
    TokenBucket limiter(5, 10);   // capacity 5, refills 10 tokens/second

    int accepted = 0, rejected = 0;
    auto start = std::chrono::steady_clock::now();

    // Burst 20 requests as fast as possible
    for (int i = 0; i < 20; ++i) {
        if (limiter.try_acquire()) {
            ++accepted;
            std::cout << "  request " << i << " accepted\n";
        } else {
            ++rejected;
            std::cout << "  request " << i << " REJECTED\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);

    std::cout << "\naccepted: " << accepted << " rejected: " << rejected
              << " in " << elapsed.count() << "ms\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Token bucket: smooth rate with burst allowance\n";
    std::cout << " * Rejected requests get immediate feedback — no blocking\n";
    std::cout << " * Refill rate controls steady-state throughput\n";
    std::cout << " * Capacity controls burst size\n";
    return 0;
}
