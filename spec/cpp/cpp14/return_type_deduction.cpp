#include <iostream>

auto add(int x, int y) {
    return x + y;
}

int main() {
    auto result = add(5, 3);
    std::cout << "Result: " << result << std::endl;
    std::cout << "Type of result: " << typeid(result).name() << std::endl;
    return 0;
}
