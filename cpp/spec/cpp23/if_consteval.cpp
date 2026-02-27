#include <iostream>
#include <type_traits>

consteval bool is_constant_evaluated() {
    return std::is_constant_evaluated();
}

constexpr int f() {
    if consteval {
        return 1;
    }
    else {
        return 0;
    }
}

int main() {
    constexpr int a = f();
    int b = f();
    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;
    return 0;
}
