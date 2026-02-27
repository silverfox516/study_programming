#include <iostream>

int main() {
    auto add = [](auto x, auto y) {
        return x + y;
    };

    std::cout << add(5, 3) << std::endl;
    std::cout << add(3.14, 2.71) << std::endl;

    return 0;
}
