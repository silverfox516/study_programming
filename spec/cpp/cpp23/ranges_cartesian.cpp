// std::views::cartesian_product — C++23
// What: produces every (a, b, c, ...) combination across input ranges.
// Why:  nested loops collapse into one declarative range expression.
// Pre:  N nested for-loops, growing in line count with the rank.
//
// libc++ feature test: __cpp_lib_ranges_cartesian_product (Apple clang 21: NOT defined)

#include <iostream>
#include <vector>
#include <string>
#include <version>

#if defined(__cpp_lib_ranges_cartesian_product) && __cpp_lib_ranges_cartesian_product >= 202207L
#  include <ranges>
#  define HAVE_CP 1
#else
#  define HAVE_CP 0
#endif

int main() {
    std::vector<int>         sizes{1, 2, 3};
    std::vector<std::string> colors{"red", "blue"};

#if HAVE_CP
    for (auto [s, c] : std::views::cartesian_product(sizes, colors)) {
        std::cout << s << " " << c << "\n";
    }
#else
    std::cout << "<ranges> std::views::cartesian_product not available in this libc++.\n";
    for (int s : sizes) {
        for (const auto& c : colors) {
            std::cout << s << " " << c << "\n";
        }
    }
#endif
    return 0;
}
