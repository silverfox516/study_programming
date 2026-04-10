// Service Locator — Architectural Pattern
//
// Intent: a central registry that lets code look up services by interface
//         type at runtime, instead of taking them as constructor arguments.
//
// When to use:
//   - You really cannot pass services through constructors (e.g., legacy plugin
//     systems, scripting hosts, very deep call trees)
//   - You want a thin global facade over a more elaborate DI container
//
// When NOT to use:
//   - 99% of cases — prefer constructor-injected DI (see dp/cpp/dependency_injection.cpp).
//     Service Locator hides dependencies, making code harder to reason about and test.
//
// This file deliberately includes the **anti-pattern critique** so the reader
// understands when to walk away from it.

#include <iostream>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>
#include <stdexcept>
#include <any>

// ============================================================================
// The locator itself
// ============================================================================
class ServiceLocator {
public:
    template <typename Interface>
    void provide(std::shared_ptr<Interface> impl) {
        services_[std::type_index(typeid(Interface))] = std::move(impl);
    }

    template <typename Interface>
    std::shared_ptr<Interface> resolve() {
        auto it = services_.find(std::type_index(typeid(Interface)));
        if (it == services_.end()) {
            throw std::runtime_error("service not registered");
        }
        return std::any_cast<std::shared_ptr<Interface>>(it->second);
    }

    void clear() { services_.clear(); }

    static ServiceLocator& global() {
        static ServiceLocator instance;
        return instance;
    }

private:
    std::unordered_map<std::type_index, std::any> services_;
};

// ============================================================================
// Example services
// ============================================================================
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& msg) = 0;
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& msg) override {
        std::cout << "[log] " << msg << "\n";
    }
};

class Greeter {
public:
    virtual ~Greeter() = default;
    virtual std::string hello(const std::string& name) = 0;
};

class FriendlyGreeter : public Greeter {
public:
    std::string hello(const std::string& name) override {
        return "Hi, " + name + "!";
    }
};

// ============================================================================
// Client code that pulls services out of the locator
// ============================================================================
void greet_user(const std::string& name) {
    auto logger  = ServiceLocator::global().resolve<Logger>();
    auto greeter = ServiceLocator::global().resolve<Greeter>();
    logger->log("greet_user called");
    std::cout << greeter->hello(name) << "\n";
}

// ============================================================================
// Demo
// ============================================================================
int main() {
    std::cout << "=== Service Locator Pattern ===\n";

    // Bootstrap: register implementations once at startup
    ServiceLocator::global().provide<Logger>(std::make_shared<ConsoleLogger>());
    ServiceLocator::global().provide<Greeter>(std::make_shared<FriendlyGreeter>());

    // Application code finds them by interface
    greet_user("Alice");
    greet_user("Bob");

    std::cout << "\nKey points:\n";
    std::cout << " * Central registry maps interface type -> implementation\n";
    std::cout << " * Client code does not need constructor parameters\n";
    std::cout << "\nWhy DI is usually preferred over Service Locator:\n";
    std::cout << " * Service Locator hides dependencies — you must read the body to find them\n";
    std::cout << " * Tests must mutate global state to swap services — fragile under parallelism\n";
    std::cout << " * Compile-time checks become runtime checks (forgot to register? crash)\n";
    std::cout << " * See dp/cpp/dependency_injection.cpp for the cleaner alternative\n";
    return 0;
}
