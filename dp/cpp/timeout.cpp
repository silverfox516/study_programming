// Timeout — Resilience Pattern
//
// Intent: bound the time a caller is willing to wait for an operation.
//         If the deadline passes, the caller gets an error immediately
//         instead of blocking forever.
//
// When to use:
//   - Downstream service may hang or respond slowly
//   - You must return to the user within a latency budget
//   - Pair with Retry (dp/cpp/retry.cpp) for retryable timeouts
//
// When NOT to use:
//   - The operation is local and guaranteed fast
//   - Cancellation is not possible (fire-and-forget work)

#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <string>
#include <stdexcept>

// ============================================================================
// Generic with_timeout wrapper
// ============================================================================
template <typename F>
auto with_timeout(F&& fn, std::chrono::milliseconds deadline) -> decltype(fn()) {
    auto future = std::async(std::launch::async, std::forward<F>(fn));
    if (future.wait_for(deadline) == std::future_status::timeout) {
        throw std::runtime_error("operation timed out");
    }
    return future.get();   // may rethrow if fn threw
}

// ============================================================================
// Demo
// ============================================================================
std::string fast_service() {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return "fast result";
}

std::string slow_service() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return "slow result";
}

int main() {
    std::cout << "=== Timeout Pattern ===\n";
    using namespace std::chrono_literals;

    // Fast call — within budget
    std::cout << "\n--- fast service (100ms budget) ---\n";
    try {
        auto r = with_timeout(fast_service, 100ms);
        std::cout << "  got: " << r << "\n";
    } catch (const std::exception& e) {
        std::cout << "  failed: " << e.what() << "\n";
    }

    // Slow call — exceeds budget
    std::cout << "\n--- slow service (100ms budget) ---\n";
    try {
        auto r = with_timeout(slow_service, 100ms);
        std::cout << "  got: " << r << "\n";
    } catch (const std::exception& e) {
        std::cout << "  failed: " << e.what() << "\n";
    }

    // Give the detached async thread time to finish to avoid sanitizer warnings
    std::this_thread::sleep_for(500ms);

    std::cout << "\nKey points:\n";
    std::cout << " * Prevents indefinite blocking on slow/hung operations\n";
    std::cout << " * Caller regains control immediately on timeout\n";
    std::cout << " * The timed-out task may still complete in the background\n";
    std::cout << " * For cooperative cancellation, combine with stop_token (C++20)\n";
    return 0;
}
