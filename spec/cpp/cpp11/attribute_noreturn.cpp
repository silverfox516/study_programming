// [[noreturn]] — C++11
// What: a function attribute that tells the compiler the function never returns.
// Why:  enables better diagnostics (no "control reaches end of non-void function" warning)
//       and lets the optimizer drop dead code after the call.
// Pre:  vendor-specific __attribute__((noreturn)) or __declspec(noreturn).

#include <iostream>
#include <stdexcept>
#include <cstdlib>

[[noreturn]] void fail(const char* msg) {
    std::cerr << "fatal: " << msg << "\n";
    std::exit(1);
}

[[noreturn]] void rethrow_as_runtime(const char* msg) {
    throw std::runtime_error(msg);
}

// Without [[noreturn]] the compiler would warn that this function may fall off the end.
int classify(int n) {
    if (n < 0) fail("negative not allowed");
    if (n == 0) return 0;
    return 1;
    // No diagnostic needed — fail() is marked noreturn so the compiler knows
    // that branch terminates control flow.
}

int main() {
    std::cout << "classify(5) = "  << classify(5)  << "\n";
    std::cout << "classify(0) = "  << classify(0)  << "\n";

    try {
        rethrow_as_runtime("simulated error");
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "\n";
    }
    return 0;
}
