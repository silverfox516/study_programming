#include <iostream>
#include <functional>

int subtract(int a, int b) {
    return a - b;
}

int main() {
    using namespace std::placeholders; // for _1, _2, etc.

    // Bind 1st argument to 10
    auto sub_from_10 = std::bind(subtract, 10, _1);
    std::cout << "10 - 5 = " << sub_from_10(5) << std::endl;

    // Bind 2nd argument to 10, and take 1st argument as the param
    auto sub_10 = std::bind(subtract, _1, 10);
    std::cout << "20 - 10 = " << sub_10(20) << std::endl;

    // reordering parameters
    auto reverse_sub = std::bind(subtract, _2, _1);
    std::cout << "reverse_sub(5, 15) [15 - 5] = " << reverse_sub(5, 15) << std::endl;

    return 0;
}
