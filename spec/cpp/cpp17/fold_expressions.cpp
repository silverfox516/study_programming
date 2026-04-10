// Fold expressions — C++17
// What: a compact way to apply a binary operator across all elements of a parameter pack:
//       (args + ...), (... + args), (args + ... + 0), (0 + ... + args).
// Why:  variadic templates used to require recursive function templates to "loop" over a pack.
// Pre:  hand-written recursion or initializer-list-and-comma tricks.

#include <iostream>

// Unary right fold:  (args op ...)
template <typename... Args>
auto sum(Args... args) {
    return (args + ...);
}

// Unary left fold:  (... op args)
template <typename... Args>
auto subtract_left(Args... args) {
    return (... - args);   // ((a - b) - c) - d
}

// Binary fold with an identity element — handles empty packs
template <typename... Args>
auto sum_or_zero(Args... args) {
    return (args + ... + 0);
}

// Comma fold — execute a side effect for each pack element
template <typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

// Logical fold — short-circuits at compile time
template <typename... Args>
bool all_true(Args... args) {
    return (args && ...);
}

int main() {
    std::cout << "sum(1,2,3,4)            = " << sum(1, 2, 3, 4)            << "\n";
    std::cout << "subtract_left(10,1,2,3) = " << subtract_left(10, 1, 2, 3) << "\n";
    std::cout << "sum_or_zero()           = " << sum_or_zero()              << "\n";

    print_all(1, "hello", 3.14, 'X');

    std::cout << "all_true(true,true,true) = " << all_true(true, true, true)  << "\n";
    std::cout << "all_true(true,false)     = " << all_true(true, false)       << "\n";

    return 0;
}
