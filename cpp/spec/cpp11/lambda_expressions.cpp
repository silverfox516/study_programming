#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Simple lambda
    std::for_each(vec.begin(), vec.end(), [](int i) {
        std::cout << i << " ";
    });
    std::cout << std::endl;

    // Lambda with capture
    int x = 10;
    auto add_x = [x](int i) {
        return i + x;
    };
    std::cout << "add_x(5): " << add_x(5) << std::endl;

    // Lambda with mutable capture
    int y = 20;
    auto change_y = [&y]() {
        y = 30;
    };
    change_y();
    std::cout << "y: " << y << std::endl;

    return 0;
}
