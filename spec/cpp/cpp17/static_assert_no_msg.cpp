// C++17: static_assert without message — the message string is now optional

#include <iostream>
#include <type_traits>

template <typename T>
T safe_divide(T a, T b) {
    // C++11 required a message
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");

    // C++17: message is optional — the condition itself is descriptive enough
    static_assert(std::is_arithmetic_v<T>);
    static_assert(!std::is_same_v<T, bool>);

    return a / b;
}

// Compile-time checks without verbose messages
static_assert(sizeof(int) >= 4);
static_assert(sizeof(long long) >= 8);
static_assert(alignof(double) >= 4);

template <typename T>
struct OnlyIntegral {
    static_assert(std::is_integral_v<T>);  // no message needed
    T value;
};

int main() {
    std::cout << "10 / 3 = " << safe_divide(10, 3) << "\n";
    std::cout << "10.0 / 3.0 = " << safe_divide(10.0, 3.0) << "\n";

    OnlyIntegral<int> a{42};
    OnlyIntegral<long> b{100};
    std::cout << "a=" << a.value << ", b=" << b.value << "\n";

    // OnlyIntegral<double> c{3.14};  // compile error: static_assert failed

    return 0;
}
