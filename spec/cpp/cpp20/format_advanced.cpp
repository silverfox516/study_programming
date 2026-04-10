// std::format advanced — C++20
// What: customizing std::format for your own types by specializing std::formatter.
// Why:  the same {} mini-language other types use, with no operator<< glue code.
// Pre:  operator<< overloads for streams; no way to plug into printf-style format strings.
// See also: spec/cpp/cpp20/format.cpp covers basic usage.

#include <iostream>
#include <format>
#include <string>

struct Point {
    int x;
    int y;
};

// Specialize std::formatter for Point to participate in {} format strings
template <>
struct std::formatter<Point> {
    // Parse the format spec — minimal implementation accepts only {}
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    // Format the value into the output iterator
    auto format(const Point& p, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};

int main() {
    // Built-in spec language: width, alignment, padding, precision
    std::cout << std::format("[{:>10}]\n", "right");
    std::cout << std::format("[{:<10}]\n", "left");
    std::cout << std::format("[{:^10}]\n", "center");
    std::cout << std::format("[{:0>5}]\n", 42);
    std::cout << std::format("[{:.3f}]\n", 3.14159);
    std::cout << std::format("[{:#x}]\n",  255);
    std::cout << std::format("[{:b}]\n",   13);

    // Custom type via the formatter specialization above
    Point p{1, 2};
    std::cout << std::format("point = {}\n", p);
    std::cout << std::format("two points: {}, {}\n", p, Point{3, 4});

    return 0;
}
