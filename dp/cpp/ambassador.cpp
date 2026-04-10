// Ambassador — Cloud Pattern
//
// Intent: a helper proxy that sits between the application and an external
//         service. The ambassador handles cross-cutting concerns (retry,
//         logging, circuit breaking, auth) so the application stays clean.
//
// When to use:
//   - Multiple services share the same client-side concerns
//   - You want to test the application without the real external service
//   - You want to inject concerns (metrics, tracing) without changing the client
//
// When NOT to use:
//   - The service call is trivial and direct is fine
//   - Latency from the extra hop is unacceptable
//   - There is only one caller (just put the logic in the caller)

#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <stdexcept>

// ============================================================================
// External service interface
// ============================================================================
class ExternalApi {
public:
    virtual ~ExternalApi() = default;
    virtual std::string call(const std::string& request) = 0;
};

// ============================================================================
// Real external service (sometimes flaky)
// ============================================================================
class RealExternalApi : public ExternalApi {
public:
    std::string call(const std::string& request) override {
        ++call_count_;
        if (call_count_ % 3 == 0) throw std::runtime_error("service unavailable");
        return "response to '" + request + "'";
    }
private:
    int call_count_ = 0;
};

// ============================================================================
// Ambassador: adds retry + logging around the real service
// ============================================================================
class AmbassadorApi : public ExternalApi {
public:
    AmbassadorApi(std::unique_ptr<ExternalApi> real, int max_retries = 3)
        : real_(std::move(real)), max_retries_(max_retries) {}

    std::string call(const std::string& request) override {
        for (int attempt = 1; attempt <= max_retries_; ++attempt) {
            try {
                std::cout << "  [ambassador] attempt " << attempt << " for '" << request << "'\n";
                auto result = real_->call(request);
                std::cout << "  [ambassador] success\n";
                return result;
            } catch (const std::exception& e) {
                std::cout << "  [ambassador] failed: " << e.what() << "\n";
                if (attempt == max_retries_) throw;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        throw std::logic_error("unreachable");
    }

private:
    std::unique_ptr<ExternalApi> real_;
    int                          max_retries_;
};

// ============================================================================
// Application code — clean, no retry/logging logic
// ============================================================================
void app_logic(ExternalApi& api) {
    try {
        std::cout << api.call("get-user") << "\n";
        std::cout << api.call("get-orders") << "\n";
        std::cout << api.call("get-balance") << "\n";
        std::cout << api.call("get-settings") << "\n";
    } catch (const std::exception& e) {
        std::cout << "  app: gave up after retries: " << e.what() << "\n";
    }
}

int main() {
    std::cout << "=== Ambassador Pattern ===\n";
    auto real = std::make_unique<RealExternalApi>();
    AmbassadorApi ambassador(std::move(real), 2);
    app_logic(ambassador);

    std::cout << "\nKey points:\n";
    std::cout << " * Application code sees a clean ExternalApi interface\n";
    std::cout << " * Ambassador handles retry, logging, metrics, auth, circuit breaking\n";
    std::cout << " * Same ambassador can wrap different external services\n";
    std::cout << " * In Kubernetes this can be a literal sidecar container\n";
    return 0;
}
