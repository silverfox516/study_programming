// consteval propagation edges — C++20
// What: a consteval function MUST be evaluated at compile time. It cannot be called
//       from a regular runtime context, even via a constexpr function.
// Why:  this example highlights the rules so the difference between consteval,
//       constexpr, and constinit is concrete.
// Pre:  no concept of "must be compile-time"; only "may be compile-time" via constexpr.
// See also: spec/cpp/cpp20/consteval.cpp

#include <iostream>

consteval int square_cval(int n) { return n * n; }
constexpr int square_cxpr(int n) { return n * n; }

// A constexpr function may be called at runtime — so it cannot internally call
// a consteval function with a runtime argument:
constexpr int doubled_cxpr(int n) {
    // return square_cval(n);  // ERROR: n is not a constant expression here
    return square_cxpr(n);     // OK
}

// But a consteval caller can pass its argument to another consteval callee
consteval int doubled_cval(int n) {
    return square_cval(n);     // OK — both are evaluated at compile time
}

int main() {
    // Pure compile-time evaluation
    constexpr int a = square_cval(5);
    constexpr int b = doubled_cval(7);
    std::cout << "square_cval(5)  = " << a << "\n";
    std::cout << "doubled_cval(7) = " << b << "\n";

    // square_cxpr can be either runtime or compile-time
    int runtime_n = 6;
    std::cout << "square_cxpr(runtime 6) = " << square_cxpr(runtime_n) << "\n";

    // Calling a consteval function with a runtime value would fail to compile:
    // square_cval(runtime_n);

    return 0;
}
