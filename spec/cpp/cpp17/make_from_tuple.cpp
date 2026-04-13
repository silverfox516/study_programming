// C++17: std::make_from_tuple — construct an object from a tuple of arguments

#include <iostream>
#include <tuple>
#include <string>

struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {
        std::cout << "  Point(" << x << ", " << y << ")\n";
    }
};

struct Person {
    std::string name;
    int age;
    double height;
    Person(std::string n, int a, double h)
        : name(std::move(n)), age(a), height(h) {
        std::cout << "  Person(" << name << ", " << age << ", " << height << ")\n";
    }
};

// Factory using make_from_tuple
template <typename T, typename Tuple>
T create(Tuple&& args) {
    return std::make_from_tuple<T>(std::forward<Tuple>(args));
}

int main() {
    // Construct from tuple
    std::cout << "make_from_tuple<Point>:\n";
    auto args1 = std::make_tuple(10, 20);
    auto p = std::make_from_tuple<Point>(args1);
    std::cout << "  result: (" << p.x << ", " << p.y << ")\n";

    // With different types
    std::cout << "\nmake_from_tuple<Person>:\n";
    auto args2 = std::make_tuple(std::string("Alice"), 30, 1.65);
    auto person = std::make_from_tuple<Person>(args2);

    // Generic factory
    std::cout << "\ncreate<Point>:\n";
    auto p2 = create<Point>(std::make_tuple(5, 15));
    std::cout << "  result: (" << p2.x << ", " << p2.y << ")\n";

    // Pair as tuple
    std::cout << "\nFrom pair:\n";
    auto pair_args = std::make_pair(99, 77);
    auto p3 = std::make_from_tuple<Point>(pair_args);
    std::cout << "  result: (" << p3.x << ", " << p3.y << ")\n";

    return 0;
}
