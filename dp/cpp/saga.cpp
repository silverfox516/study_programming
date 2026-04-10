// Saga — Distributed Transaction Coordination Pattern
//
// Intent: a sequence of local transactions where each step has a compensating
//         action. If step N fails, compensations for steps N-1 .. 1 run in
//         reverse to undo the partial work.
//
// When to use:
//   - Multiple services/databases must coordinate but 2PC is too expensive
//   - You can define a compensating action for every forward step
//   - Eventual consistency is acceptable
//
// When NOT to use:
//   - Strong consistency (ACID) is required — use a real distributed transaction
//   - Compensation is not possible (e.g., sending an email cannot be unsent)

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

// ============================================================================
// Saga orchestrator
// ============================================================================
class SagaOrchestrator {
public:
    struct Step {
        std::string              name;
        std::function<void()>    execute;
        std::function<void()>    compensate;
    };

    void add_step(Step step) { steps_.push_back(std::move(step)); }

    bool run() {
        std::vector<int> completed;
        for (int i = 0; i < static_cast<int>(steps_.size()); ++i) {
            try {
                std::cout << "  [saga] step " << steps_[i].name << " ...\n";
                steps_[i].execute();
                completed.push_back(i);
            } catch (const std::exception& e) {
                std::cout << "  [saga] FAILED at " << steps_[i].name
                          << ": " << e.what() << "\n";
                compensate(completed);
                return false;
            }
        }
        std::cout << "  [saga] all steps succeeded\n";
        return true;
    }

private:
    void compensate(const std::vector<int>& completed) {
        std::cout << "  [saga] compensating...\n";
        for (auto it = completed.rbegin(); it != completed.rend(); ++it) {
            try {
                std::cout << "  [saga] undo " << steps_[*it].name << "\n";
                steps_[*it].compensate();
            } catch (const std::exception& e) {
                std::cout << "  [saga] compensation failed for " << steps_[*it].name
                          << ": " << e.what() << " (manual intervention needed)\n";
            }
        }
    }

    std::vector<Step> steps_;
};

// ============================================================================
// Demo: booking a trip (flight + hotel + car)
// ============================================================================
int main() {
    std::cout << "=== Saga Pattern ===\n";

    bool flight_booked = false;
    bool hotel_booked  = false;

    // Successful saga
    {
        SagaOrchestrator saga;
        saga.add_step({"book flight",
            [&] { flight_booked = true; std::cout << "    flight booked\n"; },
            [&] { flight_booked = false; std::cout << "    flight cancelled\n"; }});
        saga.add_step({"book hotel",
            [&] { hotel_booked = true; std::cout << "    hotel booked\n"; },
            [&] { hotel_booked = false; std::cout << "    hotel cancelled\n"; }});
        saga.add_step({"book car",
            [&] { std::cout << "    car booked\n"; },
            [&] { std::cout << "    car cancelled\n"; }});

        std::cout << "\n--- happy path ---\n";
        saga.run();
    }

    // Failing saga — car rental fails, flight + hotel must be compensated
    {
        flight_booked = false;
        hotel_booked  = false;

        SagaOrchestrator saga;
        saga.add_step({"book flight",
            [&] { flight_booked = true; std::cout << "    flight booked\n"; },
            [&] { flight_booked = false; std::cout << "    flight cancelled\n"; }});
        saga.add_step({"book hotel",
            [&] { hotel_booked = true; std::cout << "    hotel booked\n"; },
            [&] { hotel_booked = false; std::cout << "    hotel cancelled\n"; }});
        saga.add_step({"book car",
            [&] { throw std::runtime_error("no cars available"); },
            [&] { std::cout << "    car cancelled\n"; }});

        std::cout << "\n--- failure path ---\n";
        saga.run();
        std::cout << "  flight_booked=" << flight_booked
                  << " hotel_booked=" << hotel_booked << "\n";
    }

    std::cout << "\nKey points:\n";
    std::cout << " * Each step defines execute() and compensate()\n";
    std::cout << " * On failure, compensation runs in reverse order\n";
    std::cout << " * Compensation itself can fail — needs manual fallback\n";
    std::cout << " * Choreography (event-based) is an alternative to orchestration\n";
    return 0;
}
