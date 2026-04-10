#include <iostream>
#include <vector>
#include <algorithm>

int subtract(int a, int b) {
    return a - b;
}

int main() {
    // In C++14, generic lambdas (auto parameters) make std::bind obsolete.
    
    // Equivalent to bind(subtract, 10, _1)
    auto sub_from_10 = [](auto b) { return subtract(10, b); };
    std::cout << "10 - 5 = " << sub_from_10(5) << std::endl;

    // Equivalent to bind(subtract, _1, 10)
    auto sub_10 = [](auto a) { return subtract(a, 10); };
    std::cout << "20 - 10 = " << sub_10(20) << std::endl;

    // Polymorphic lambda
    auto print_twice = [](auto val) {
        std::cout << val << val << std::endl;
    };
    
    print_twice(10);
    print_twice("Hello");

    return 0;
}
