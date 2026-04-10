// Relaxed constexpr — C++14
// What: constexpr functions can now contain multiple statements, local variables,
//       loops, and mutations of those locals.
// Why:  C++11 constexpr was limited to a single return expression — too restrictive
//       for anything beyond trivial formulas.
// Pre:  C++11 constexpr forced everything into one ternary or recursive call chain.

#include <iostream>

// C++11-style: had to be a single expression
constexpr int factorial_11(int n) {
    return n <= 1 ? 1 : n * factorial_11(n - 1);
}

// C++14-style: imperative loop, local mutation, all at compile time
constexpr int factorial_14(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// More substantial: compile-time linear search
constexpr int find_index(const int* arr, int size, int target) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == target) return i;
    }
    return -1;
}

int main() {
    // Both are evaluated at compile time when the inputs are constant expressions.
    constexpr int a = factorial_11(6);
    constexpr int b = factorial_14(6);

    static_assert(a == 720, "");
    static_assert(b == 720, "");

    constexpr int data[] = {10, 20, 30, 40, 50};
    constexpr int idx = find_index(data, 5, 30);
    static_assert(idx == 2, "");

    std::cout << "factorial_11(6) = " << a   << "\n";
    std::cout << "factorial_14(6) = " << b   << "\n";
    std::cout << "find_index(30)  = " << idx << "\n";

    return 0;
}
