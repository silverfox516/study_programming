// std::views::enumerate — C++23
// What: yields tuples of (index, element) over an underlying range.
// Why:  the Pythonic equivalent of `for index, value in enumerate(seq)`.
//       Removes the boilerplate index variable that range-for forced you to skip.
// Pre:  hand-written counter incremented inside a range-for body.
//
// libc++ feature test: __cpp_lib_ranges_enumerate (Apple clang 21: NOT defined)

#include <iostream>
#include <vector>
#include <string>
#include <version>

#if defined(__cpp_lib_ranges_enumerate) && __cpp_lib_ranges_enumerate >= 202302L
#  include <ranges>
#  define HAVE_ENUM 1
#else
#  define HAVE_ENUM 0
#endif

int main() {
    std::vector<std::string> items{"apple", "banana", "cherry"};

#if HAVE_ENUM
    for (auto [i, item] : std::views::enumerate(items)) {
        std::cout << i << ": " << item << "\n";
    }
#else
    std::cout << "<ranges> std::views::enumerate not available in this libc++.\n";
    for (std::size_t i = 0; i < items.size(); ++i) {
        std::cout << i << ": " << items[i] << "\n";
    }
#endif
    return 0;
}
