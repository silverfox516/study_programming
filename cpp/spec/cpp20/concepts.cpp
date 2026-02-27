#include <iostream>
#include <concepts>

template <typename T>
concept Integral = std::is_integral_v<T>;

template <Integral T>
T add(T a, T b) {
    return a + b;
}

int main() {
    std::cout << add(5, 3) << std::endl;
    // std::cout << add(3.14, 2.71) << std::endl; // Error: double does not satisfy Integral concept
    return 0;
}
