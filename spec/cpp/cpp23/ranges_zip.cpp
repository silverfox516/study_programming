// std::views::zip — C++23
// What: combine multiple ranges element-wise into a range of tuples, stopping at
//       the shortest input.
// Why:  the canonical "iterate two parallel arrays together" pattern, finally
//       in the standard library.
// Pre:  hand-written index loops referencing both ranges by `[i]`.
//
// libc++ feature test: __cpp_lib_ranges_zip (Apple clang 21 reports NOT defined)

#include <iostream>
#include <vector>
#include <string>
#include <version>

#if defined(__cpp_lib_ranges_zip) && __cpp_lib_ranges_zip >= 202110L
#  include <ranges>
#  define HAVE_ZIP 1
#else
#  define HAVE_ZIP 0
#endif

int main() {
#if HAVE_ZIP
    std::vector<std::string> names{"Alice", "Bob", "Carol"};
    std::vector<int>         ages{30, 25, 27};

    for (auto [name, age] : std::views::zip(names, ages)) {
        std::cout << name << " is " << age << "\n";
    }
#else
    std::cout << "<ranges> std::views::zip not available in this libc++.\n";
    std::cout << "Workaround: index-based loop over the shorter range.\n";

    std::vector<std::string> names{"Alice", "Bob", "Carol"};
    std::vector<int>         ages{30, 25, 27};
    auto n = std::min(names.size(), ages.size());
    for (std::size_t i = 0; i < n; ++i) {
        std::cout << names[i] << " is " << ages[i] << "\n";
    }
#endif
    return 0;
}
