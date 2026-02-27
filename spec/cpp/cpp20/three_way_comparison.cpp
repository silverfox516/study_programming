#include <iostream>
#include <compare>

struct Point {
    int x, y;
    auto operator<=>(const Point& other) const = default;
};

int main() {
    Point p1{1, 2}, p2{1, 3};

    auto result = p1 <=> p2;

    if (result < 0) {
        std::cout << "p1 < p2" << std::endl;
    } else if (result > 0) {
        std::cout << "p1 > p2" << std::endl;
    } else {
        std::cout << "p1 == p2" << std::endl;
    }

    return 0;
}
