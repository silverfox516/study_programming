// Retry — Resilience Pattern (Cloud Design Patterns)
//
// Intent: automatically re-attempt a failed operation, with configurable
//         back-off, max attempts, and jitter. Transient failures (network
//         blips, temporary overload) often self-heal within seconds.
//
// When to use:
//   - The failure is likely transient (timeout, 503, connection reset)
//   - The operation is idempotent (safe to repeat)
//   - Combined with Circuit Breaker (dp/cpp/circuit_breaker.cpp)
//
// When NOT to use:
//   - The error is deterministic (400 Bad Request, auth failure)
//   - The operation has side effects that cannot be safely repeated

#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>
#include <string>

// ============================================================================
// Retry policy
// ============================================================================
struct RetryPolicy {
    int    max_attempts      = 3;
    int    base_delay_ms     = 100;
    double backoff_multiplier = 2.0;
    bool   add_jitter        = true;
};

template <typename F>
auto retry(F&& fn, const RetryPolicy& policy = {}) -> decltype(fn()) {
    std::mt19937 rng(std::random_device{}());
    int delay_ms = policy.base_delay_ms;

    for (int attempt = 1; attempt <= policy.max_attempts; ++attempt) {
        try {
            return fn();
        } catch (const std::exception& e) {
            std::cout << "  attempt " << attempt << " failed: " << e.what() << "\n";
            if (attempt == policy.max_attempts) throw;

            int actual_delay = delay_ms;
            if (policy.add_jitter) {
                std::uniform_int_distribution<int> jitter(0, delay_ms / 2);
                actual_delay += jitter(rng);
            }
            std::cout << "  waiting " << actual_delay << "ms...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(actual_delay));
            delay_ms = static_cast<int>(delay_ms * policy.backoff_multiplier);
        }
    }
    throw std::logic_error("unreachable");
}

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Retry Pattern ===\n";

    // Simulate a flaky service that succeeds on the 3rd call
    int call_count = 0;
    auto flaky = [&]() -> std::string {
        ++call_count;
        if (call_count < 3) throw std::runtime_error("transient error");
        return "success on attempt " + std::to_string(call_count);
    };

    std::cout << "\n--- flaky service (succeeds on 3rd) ---\n";
    auto result = retry(flaky, {.max_attempts = 5, .base_delay_ms = 50});
    std::cout << "  result: " << result << "\n";

    // Simulate a permanent failure
    std::cout << "\n--- permanent failure ---\n";
    try {
        retry([]() -> int { throw std::runtime_error("always fails"); },
              {.max_attempts = 3, .base_delay_ms = 30, .add_jitter = false});
    } catch (const std::exception& e) {
        std::cout << "  gave up: " << e.what() << "\n";
    }

    std::cout << "\nKey points:\n";
    std::cout << " * Exponential backoff prevents thundering herds\n";
    std::cout << " * Jitter spreads retries across time\n";
    std::cout << " * Always cap max_attempts to avoid infinite loops\n";
    std::cout << " * Only retry idempotent, transient-failure-prone operations\n";
    std::cout << " * Pair with circuit_breaker.cpp to stop retrying when failure is systemic\n";
    return 0;
}
