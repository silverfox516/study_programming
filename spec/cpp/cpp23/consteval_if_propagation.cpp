// `if consteval` interaction with consteval functions — C++23
// What: `if consteval { ... }` selects a branch when the surrounding evaluation
//       is itself a constant evaluation. Within that branch, you may call
//       consteval functions even with expressions that look like runtime values.
// Why:  lets a single function provide both compile-time and runtime impls
//       cleanly, with no SFINAE.
// Pre:  std::is_constant_evaluated() returned a bool you had to test, but you
//       could not call consteval functions inside the "true" branch.
// See also: spec/cpp/cpp23/if_consteval.cpp may already cover the basics.

#include <iostream>

consteval int compile_time_only(int n) { return n * 100; }
constexpr int runtime_only(int n)      { return n * 1; }

constexpr int either(int n) {
    if consteval {
        // This branch is selected when 'either' is itself in a constant context.
        // Inside it, n is treated as constant — we can call consteval functions.
        return compile_time_only(n);
    } else {
        return runtime_only(n);
    }
}

int main() {
    constexpr int compile_path = either(5);   // -> compile_time_only -> 500
    static_assert(compile_path == 500, "");

    int runtime_n = 5;
    int runtime_path = either(runtime_n);     // -> runtime_only -> 5
    std::cout << "compile path: " << compile_path << "\n";
    std::cout << "runtime path: " << runtime_path << "\n";

    return 0;
}
