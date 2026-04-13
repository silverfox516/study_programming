// C++17: constexpr lambda — lambdas are implicitly constexpr when possible

#include <iostream>
#include <array>

int main() {
    // lambda is implicitly constexpr
    constexpr auto square = [](int n) { return n * n; };
    static_assert(square(5) == 25);

    // explicit constexpr lambda
    constexpr auto add = [](int a, int b) constexpr { return a + b; };
    static_assert(add(3, 4) == 7);

    // use in compile-time context
    constexpr auto factorial = [](int n) {
        int result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        return result;
    };
    static_assert(factorial(5) == 120);

    // constexpr lambda to initialize array size
    constexpr auto size = [](int n) { return n * 2; };
    std::array<int, size(4)> arr{};  // array of 8 elements
    std::cout << "Array size: " << arr.size() << "\n";

    // runtime usage still works
    int x = 7;
    std::cout << "square(7) = " << square(x) << "\n";
    std::cout << "factorial(5) = " << factorial(5) << "\n";

    return 0;
}
