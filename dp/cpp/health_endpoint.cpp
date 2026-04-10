// Health Endpoint — Cloud Pattern
//
// Intent: expose a dedicated endpoint (like /health or /ready) that reports
//         the service's operational status. Load balancers, orchestrators,
//         and monitoring systems poll it to decide routing and alerting.
//
// When to use:
//   - Kubernetes liveness / readiness probes
//   - Load balancer health checks
//   - Monitoring dashboards
//
// When NOT to use:
//   - The service has no network surface to expose (batch jobs, CLI tools)

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>

// ============================================================================
// Health check model
// ============================================================================
enum class Status { Up, Down, Degraded };

std::string to_string(Status s) {
    switch (s) {
    case Status::Up:       return "UP";
    case Status::Down:     return "DOWN";
    case Status::Degraded: return "DEGRADED";
    }
    return "UNKNOWN";
}

struct CheckResult {
    std::string name;
    Status      status;
    std::string detail;
};

// ============================================================================
// Health checker — aggregates multiple checks
// ============================================================================
class HealthChecker {
public:
    using Check = std::function<CheckResult()>;

    void add_check(Check c) { checks_.push_back(std::move(c)); }

    struct Report {
        Status                   overall;
        std::vector<CheckResult> details;
    };

    Report run() const {
        Report report{Status::Up, {}};
        for (const auto& c : checks_) {
            auto result = c();
            report.details.push_back(result);
            if (result.status == Status::Down) {
                report.overall = Status::Down;
            } else if (result.status == Status::Degraded && report.overall == Status::Up) {
                report.overall = Status::Degraded;
            }
        }
        return report;
    }

private:
    std::vector<Check> checks_;
};

// ============================================================================
// Demo checks
// ============================================================================
CheckResult check_database() {
    return {"database", Status::Up, "connected, 5ms latency"};
}

CheckResult check_cache() {
    return {"cache", Status::Degraded, "high miss rate (82%)"};
}

CheckResult check_disk() {
    return {"disk", Status::Up, "42% used"};
}

int main() {
    std::cout << "=== Health Endpoint Pattern ===\n";
    HealthChecker checker;
    checker.add_check(check_database);
    checker.add_check(check_cache);
    checker.add_check(check_disk);

    auto report = checker.run();

    // Simulate JSON-like output
    std::cout << "{\n";
    std::cout << "  \"status\": \"" << to_string(report.overall) << "\",\n";
    std::cout << "  \"checks\": [\n";
    for (std::size_t i = 0; i < report.details.size(); ++i) {
        const auto& d = report.details[i];
        std::cout << "    {\"name\": \"" << d.name
                  << "\", \"status\": \"" << to_string(d.status)
                  << "\", \"detail\": \"" << d.detail << "\"}";
        if (i + 1 < report.details.size()) std::cout << ",";
        std::cout << "\n";
    }
    std::cout << "  ]\n}\n";

    std::cout << "\nKey points:\n";
    std::cout << " * Aggregate multiple subsystem checks into one status\n";
    std::cout << " * Kubernetes: liveness (restart), readiness (stop routing)\n";
    std::cout << " * Keep checks fast — health probes run frequently\n";
    std::cout << " * Expose as HTTP GET /health, return 200/503 based on status\n";
    return 0;
}
