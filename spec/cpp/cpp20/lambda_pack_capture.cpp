// Lambda init-capture pack expansion — C++20
// What: `[...args = std::move(args)]` captures a parameter pack into a lambda
//       by perfect-forwarding each element into a corresponding init-capture.
// Why:  lets a forwarding factory hold a variadic pack inside a lambda, which is
//       essential for thread launching, async continuations, and closure-based DSLs.
// Pre:  workarounds with std::tuple captures and std::apply at the call site.

#include <iostream>
#include <utility>
#include <string>
#include <memory>

template <typename... Args>
auto make_printer(Args... args) {
    // Capture each pack element by move into a corresponding init-capture
    return [...captured = std::move(args)]() {
        // Use a fold expression over the captured pack
        ((std::cout << captured << " "), ...);
        std::cout << "\n";
    };
}

template <typename... Args>
auto make_size_reporter(Args... args) {
    // Same idea, but use a fold over a different operation
    return [...captured = std::move(args)]() {
        std::size_t total = (captured.size() + ...);
        std::cout << "total chars: " << total << "\n";
    };
}

int main() {
    auto p1 = make_printer(1, "two", 3.0, std::string("four"));
    p1();
    p1();    // can be called many times — captures are stored

    // The pack capture itself is what's new in C++20.
    // Move-only types in the captured pack work too:
    auto up1 = std::make_unique<int>(7);
    auto up2 = std::make_unique<int>(8);
    auto p3 = [u1 = std::move(up1), u2 = std::move(up2)]() {
        std::cout << "moved-in unique_ptrs: " << *u1 << " " << *u2 << "\n";
    };
    p3();

    auto reporter = make_size_reporter(std::string("hello"),
                                       std::string("world"),
                                       std::string("!"));
    reporter();

    return 0;
}
