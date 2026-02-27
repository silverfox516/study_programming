#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <string>
#include <stdexcept>

enum class CircuitState {
    Closed,
    Open,
    HalfOpen
};

class CircuitBreakerException : public std::runtime_error {
public:
    CircuitBreakerException(const std::string& msg) : std::runtime_error(msg) {}
};

class CircuitBreaker {
private:
    CircuitState state;
    int failureCount;
    int failureThreshold;
    std::chrono::time_point<std::chrono::steady_clock> lastFailureTime;
    std::chrono::seconds recoveryTimeout;

public:
    CircuitBreaker(int threshold, int timeoutSec) 
        : state(CircuitState::Closed), 
          failureCount(0), 
          failureThreshold(threshold), 
          recoveryTimeout(timeoutSec) {}

    template<typename Func>
    void execute(Func action) {
        if (state == CircuitState::Open) {
            auto now = std::chrono::steady_clock::now();
            if (now - lastFailureTime > recoveryTimeout) {
                std::cout << "[Circuit] Timeout reached. Switching to HALF-OPEN." << std::endl;
                state = CircuitState::HalfOpen;
            } else {
                throw CircuitBreakerException("Circuit is OPEN. Fast failing.");
            }
        }

        try {
            action();
            
            if (state == CircuitState::HalfOpen) {
                std::cout << "[Circuit] Success in HALF-OPEN. Closing." << std::endl;
                reset();
            } else if (failureCount > 0) {
                failureCount = 0; // Reset on success in Closed state
                std::cout << "[Circuit] Success. Failure count reset." << std::endl;
            }
            
        } catch (const std::exception& e) {
            handleFailure();
            throw; // Re-throw
        }
    }

private:
    void handleFailure() {
        failureCount++;
        lastFailureTime = std::chrono::steady_clock::now();
        std::cout << "[Circuit] Failure recorded. Count: " << failureCount << std::endl;

        if (state == CircuitState::HalfOpen) {
             std::cout << "[Circuit] Failed in HALF-OPEN. Opening again." << std::endl;
             state = CircuitState::Open;
        } else if (failureCount >= failureThreshold) {
             std::cout << "[Circuit] Threshold reached. Opening circuit." << std::endl;
             state = CircuitState::Open;
        }
    }

    void reset() {
        state = CircuitState::Closed;
        failureCount = 0;
    }
};

// --- Simulation ---

void reliableService() {
    std::cout << "Service call success!" << std::endl;
}

void failingService() {
    throw std::runtime_error("Connection timeout");
}

int main() {
    std::cout << "=== Circuit Breaker Pattern (C++) ===" << std::endl;

    CircuitBreaker cb(2, 2); // 2 failures allowed, 2 seconds timeout

    std::cout << "\n1. Normal Operation" << std::endl;
    cb.execute(reliableService);

    std::cout << "\n2. Failures Occur" << std::endl;
    try { cb.execute(failingService); } catch (...) {}
    try { cb.execute(failingService); } catch (...) {} // Trips here

    std::cout << "\n3. Fast Fail (Open State)" << std::endl;
    try { 
        cb.execute(reliableService); // Should fail immediately even if service is fixed
    } catch (const CircuitBreakerException& e) {
        std::cout << "Blocked: " << e.what() << std::endl;
    }

    std::cout << "\nWaiting for recovery..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "\n4. Half-Open Recovery" << std::endl;
    cb.execute(reliableService); // Should succeed and close circuit

    std::cout << "\n5. Back to Normal" << std::endl;
    cb.execute(reliableService);

    return 0;
}
